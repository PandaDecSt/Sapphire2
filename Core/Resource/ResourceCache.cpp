#include "../../Sapphire/Predefined.h"

#include "../Core/Context.h"
#include "../Core/CoreEvents.h"
#include "../Core/Profiler.h"
#include "../Core/WorkQueue.h"
#include "../IO/FileSystem.h"
#include "../IO/FileWatcher.h"
#include "../IO/Log.h"
#include "../IO/PackageFile.h"
#include "../Resource/BackgroundLoader.h"
#include "../Resource/Image.h"
#include "../Resource/JSONFile.h"
#include "../Resource/PListFile.h"
#include "../Resource/ResourceCache.h"
#include "../Resource/ResourceEvents.h"
#include "../Resource/XMLFile.h"

#include "../DebugNew.h"

#include <cstdio>


namespace  Sapphire
{
	//��ԴĿ¼��
	static const char* checkDirs[] =
	{
		"Fonts",
		"Materials",
		"Models",
		"Music",
		"Objects",
		"Particle",
		"PostProcess",
		"RenderPaths",
		"Scenes",
		"Scripts",
		"Sounds",
		"Shaders",
		"Techniques",
		"Textures",
		"UI",
		0
	};

	static const SharedPtr<Resource> noResource;

	ResourceCache::ResourceCache(Context* context) :
		Object(context),
		autoReloadResources_(false),
		returnFailedResources_(false),
		searchPackagesFirst_(true),
		isRouting_(false),
		finishBackgroundResourcesMs_(5)
	{
		// ע����Դ����󹤳�
		RegisterResourceLibrary(context_);

#ifdef SAPPHIRE_THREADING
		//������̨�������������߳̽����ڵ�һ�κ�̨����ʱ����
		backgroundLoader_ = new BackgroundLoader(this);
#endif

		// ����֡��ʼ�¼�����Ŀ¼�۲��ߺͺ�̨��Դ����
		SubscribeToEvent(E_BEGINFRAME, SAPPHIRE_HANDLER(ResourceCache, HandleBeginFrame));
	}

	ResourceCache::~ResourceCache()
	{
#ifdef SAPPHIRE_THREADING
		backgroundLoader_.Reset();
#endif
	}

	bool ResourceCache::AddResourceDir(const String& pathName, unsigned priority)
	{
		MutexLock lock(resourceMutex_);

		FileSystem* fileSystem = GetSubsystem<FileSystem>();
		if (!fileSystem || !fileSystem->DirExists(pathName))
		{
			SAPPHIRE_LOGERROR("Could not open directory " + pathName);
			return false;
		}

		// תΪ����·��
		String fixedPath = SanitateResourceDirName(pathName);

		// ����Ƿ���ͬ����Ŀ¼
		for (unsigned i = 0; i < resourceDirs_.Size(); ++i)
		{
			if (!resourceDirs_[i].Compare(fixedPath, false))
				return true;
		}
		//�����ȼ���
		if (priority < resourceDirs_.Size())
			resourceDirs_.Insert(priority, fixedPath);
		else
			resourceDirs_.Push(fixedPath); //�����

		// ��������Զ����أ� Ϊ���Ŀ¼����һ���ļ��۲���
		if (autoReloadResources_)
		{
			SharedPtr<FileWatcher> watcher(new FileWatcher(context_));
			watcher->StartWatching(fixedPath, true);
			fileWatchers_.Push(watcher);
		}

		SAPPHIRE_LOGINFO("Added resource path " + fixedPath);
		return true;
	}

	bool ResourceCache::AddPackageFile(PackageFile* package, unsigned priority)
	{
		MutexLock lock(resourceMutex_);

		if (!package || !package->GetNumFiles())
		{
			SAPPHIRE_LOGERRORF("Could not add package file %s due to load failure", package->GetName().CString());
			return false;
		}

		if (priority < packages_.Size())
			packages_.Insert(priority, SharedPtr<PackageFile>(package));
		else
			packages_.Push(SharedPtr<PackageFile>(package));

		SAPPHIRE_LOGINFO("Added resource package " + package->GetName());
		return true;
	}

	bool ResourceCache::AddPackageFile(const String& fileName, unsigned priority)
	{
		SharedPtr<PackageFile> package(new PackageFile(context_));
		return package->Open(fileName) && AddPackageFile(package);
	}

	bool ResourceCache::AddManualResource(Resource* resource)
	{
		if (!resource)
		{
			SAPPHIRE_LOGERROR("Null manual resource");
			return false;
		}

		const String& name = resource->GetName();
		if (name.Empty())
		{
			SAPPHIRE_LOGERROR("Manual resource with empty name, can not add");
			return false;
		}

		resource->ResetUseTimer();  //����ʹ�ü�ʱ�� 
		resourceGroups_[resource->GetType()].resources_[resource->GetNameHash()] = resource;
		UpdateResourceGroup(resource->GetType());
		return true;
	}

	void ResourceCache::RemoveResourceDir(const String& pathName)
	{
		MutexLock lock(resourceMutex_);

		String fixedPath = SanitateResourceDirName(pathName);  //��þ���·��

		for (unsigned i = 0; i < resourceDirs_.Size(); ++i)
		{
			if (!resourceDirs_[i].Compare(fixedPath, false))
			{
				resourceDirs_.Erase(i);
				// �Ƴ���ӦĿ¼��filewatcher
				for (unsigned j = 0; j < fileWatchers_.Size(); ++j)
				{
					if (!fileWatchers_[j]->GetPath().Compare(fixedPath, false))
					{
						fileWatchers_.Erase(j);
						break;
					}
				}
				SAPPHIRE_LOGINFO("Removed resource path " + fixedPath);
				return;
			}
		}
	}

	void ResourceCache::RemovePackageFile(PackageFile* package, bool releaseResources, bool forceRelease)
	{
		MutexLock lock(resourceMutex_);

		for (Vector<SharedPtr<PackageFile> >::Iterator i = packages_.Begin(); i != packages_.End(); ++i)
		{
			if (*i == package)
			{
				if (releaseResources)
					ReleasePackageResources(*i, forceRelease);
				SAPPHIRE_LOGINFO("Removed resource package " + (*i)->GetName());
				packages_.Erase(i);
				return;
			}
		}
	}

	void ResourceCache::RemovePackageFile(const String& fileName, bool releaseResources, bool forceRelease)
	{
		MutexLock lock(resourceMutex_);

		// �Ƚ��ļ�������չ��
		String fileNameNoPath = GetFileNameAndExtension(fileName);

		for (Vector<SharedPtr<PackageFile> >::Iterator i = packages_.Begin(); i != packages_.End(); ++i)
		{
			if (!GetFileNameAndExtension((*i)->GetName()).Compare(fileNameNoPath, false))
			{
				if (releaseResources)
					ReleasePackageResources(*i, forceRelease);
				SAPPHIRE_LOGINFO("Removed resource package " + (*i)->GetName());
				packages_.Erase(i);
				return;
			}
		}
	}

	void ResourceCache::ReleaseResource(StringHash type, const String& name, bool force)
	{
		StringHash nameHash(name);
		const SharedPtr<Resource>& existingRes = FindResource(type, nameHash);
		if (!existingRes)
			return;

		// �����������ô��ڣ���Ҫ�ͷţ�����force == true
		if ((existingRes.Refs() == 1 && existingRes.WeakRefs() == 0) || force)
		{
			resourceGroups_[type].resources_.Erase(nameHash);
			UpdateResourceGroup(type);
		}
	}

	void ResourceCache::ReleaseResources(StringHash type, bool force)
	{
		bool released = false;

		HashMap<StringHash, ResourceGroup>::Iterator i = resourceGroups_.Find(type);
		if (i != resourceGroups_.End())
		{
			for (HashMap<StringHash, SharedPtr<Resource> >::Iterator j = i->second_.resources_.Begin();
				j != i->second_.resources_.End();)
			{
				HashMap<StringHash, SharedPtr<Resource> >::Iterator current = j++;
				// �����������ô��ڣ���Ҫ�ͷţ�����force == true
				if ((current->second_.Refs() == 1 && current->second_.WeakRefs() == 0) || force)
				{
					i->second_.resources_.Erase(current);
					released = true;
				}
			}
		}

		if (released)
			UpdateResourceGroup(type);
	}

	void ResourceCache::ReleaseResources(StringHash type, const String& partialName, bool force)
	{
		bool released = false;

		HashMap<StringHash, ResourceGroup>::Iterator i = resourceGroups_.Find(type);
		if (i != resourceGroups_.End())
		{
			for (HashMap<StringHash, SharedPtr<Resource> >::Iterator j = i->second_.resources_.Begin();
				j != i->second_.resources_.End();)
			{
				HashMap<StringHash, SharedPtr<Resource> >::Iterator current = j++;
				if (current->second_->GetName().Contains(partialName))
				{
					// �����������ô��ڣ���Ҫ�ͷţ�����force == true
					if ((current->second_.Refs() == 1 && current->second_.WeakRefs() == 0) || force)
					{
						i->second_.resources_.Erase(current);
						released = true;
					}
				}
			}
		}

		if (released)
			UpdateResourceGroup(type);
	}

	void ResourceCache::ReleaseResources(const String& partialName, bool force)
	{
		// ��Щ��Դ�����������ģ������������Ҫȷ�����ֶ��ͷ�
	    // ����force==true
		unsigned repeat = force ? 1 : 2;

		while (repeat--)
		{
			for (HashMap<StringHash, ResourceGroup>::Iterator i = resourceGroups_.Begin(); i != resourceGroups_.End(); ++i)
			{
				bool released = false;

				for (HashMap<StringHash, SharedPtr<Resource> >::Iterator j = i->second_.resources_.Begin();
					j != i->second_.resources_.End();)
				{
					HashMap<StringHash, SharedPtr<Resource> >::Iterator current = j++;
					if (current->second_->GetName().Contains(partialName))
					{
						// ����б�����ô��ڣ��ͷţ�����ǿ��
						if ((current->second_.Refs() == 1 && current->second_.WeakRefs() == 0) || force)
						{
							i->second_.resources_.Erase(current);
							released = true;
						}
					}
				}
				if (released)
					UpdateResourceGroup(i->first_);
			}
		}
	}

	void ResourceCache::ReleaseAllResources(bool force)
	{
		unsigned repeat = force ? 1 : 2;

		while (repeat--)
		{
			for (HashMap<StringHash, ResourceGroup>::Iterator i = resourceGroups_.Begin();
				i != resourceGroups_.End(); ++i)
			{
				bool released = false;

				for (HashMap<StringHash, SharedPtr<Resource> >::Iterator j = i->second_.resources_.Begin();
					j != i->second_.resources_.End();)
				{
					HashMap<StringHash, SharedPtr<Resource> >::Iterator current = j++;
					// ����б�����ô��ڣ��ͷţ�����ǿ��
					if ((current->second_.Refs() == 1 && current->second_.WeakRefs() == 0) || force)
					{
						i->second_.resources_.Erase(current);
						released = true;
					}
				}
				if (released)
					UpdateResourceGroup(i->first_);
			}
		}
	}

	//������Դ
	bool ResourceCache::ReloadResource(Resource* resource)
	{
		if (!resource)
			return false;

		//������Դ�����¼�
		resource->SendEvent(E_RELOADSTARTED);

		bool success = false;
		SharedPtr<File> file = GetFile(resource->GetName());
		if (file)
			success = resource->Load(*(file.Get()));

		if (success)
		{
			resource->ResetUseTimer();   //����ʹ�ü�ʱ��
			UpdateResourceGroup(resource->GetType());
			resource->SendEvent(E_RELOADFINISHED);    //�������
			return true;
		}

		// �������ʧ�ܣ����ӻ������Ƴ������Դ��Ҫ׼�������ٴμ���
		resource->SendEvent(E_RELOADFAILED);
		return false;
	}

	void ResourceCache::ReloadResourceWithDependencies(const String& fileName)
	{
		StringHash fileNameHash(fileName);
		// ������Դ
		const SharedPtr<Resource>& resource = FindResource(fileNameHash);
		if (resource)
		{
			SAPPHIRE_LOGDEBUG("Reloading changed resource " + fileName);
			ReloadResource(resource);
		}
		// ͨ��������Դ��xml�ļ���������Դ��������ϵ
		if (!resource || GetExtension(resource->GetName()) == ".xml")
		{
			// �������������Դ��������
			HashMap<StringHash, HashSet<StringHash> >::ConstIterator j = dependentResources_.Find(fileNameHash);
			if (j != dependentResources_.End())
			{
				//����һ����Դ���ܻ��޸��������ٽṹ������ռ������Դǰ������Ҫ����
				Vector<SharedPtr<Resource> > dependents;
				dependents.Reserve(j->second_.Size());

				for (HashSet<StringHash>::ConstIterator k = j->second_.Begin(); k != j->second_.End(); ++k)
				{
					const SharedPtr<Resource>& dependent = FindResource(*k);
					if (dependent)
						dependents.Push(dependent);
				}

				for (unsigned k = 0; k < dependents.Size(); ++k)
				{
					SAPPHIRE_LOGDEBUG("Reloading resource " + dependents[k]->GetName() + " depending on " + fileName);
					ReloadResource(dependents[k]);
				}
			}
		}
	}

	void ResourceCache::SetMemoryBudget(StringHash type, unsigned long long budget)
	{
		resourceGroups_[type].memoryBudget_ = budget;
	}

	//�����Զ�������Դ
	void ResourceCache::SetAutoReloadResources(bool enable)
	{
		if (enable != autoReloadResources_)
		{
			if (enable)
			{
				//Ϊÿ��Ŀ¼����fileWatcher
				for (unsigned i = 0; i < resourceDirs_.Size(); ++i)
				{
					SharedPtr<FileWatcher> watcher(new FileWatcher(context_));
					watcher->StartWatching(resourceDirs_[i], true);
					fileWatchers_.Push(watcher);
				}
			}
			else
				fileWatchers_.Clear();

			autoReloadResources_ = enable;
		}
	}

	void ResourceCache::AddResourceRouter(ResourceRouter* router, bool addAsFirst)
	{
		// Check for duplicate
		for (unsigned i = 0; i < resourceRouters_.Size(); ++i)
		{
			if (resourceRouters_[i] == router)
				return;
		}

		if (addAsFirst)
			resourceRouters_.Insert(0, SharedPtr<ResourceRouter>(router));
		else
			resourceRouters_.Push(SharedPtr<ResourceRouter>(router));
	}

	void ResourceCache::RemoveResourceRouter(ResourceRouter* router)
	{
		for (unsigned i = 0; i < resourceRouters_.Size(); ++i)
		{
			if (resourceRouters_[i] == router)
			{
				resourceRouters_.Erase(i);
				return;
			}
		}
	}

	SharedPtr<File> ResourceCache::GetFile(const String& nameIn, bool sendEventOnFailure)
	{
		MutexLock lock(resourceMutex_);

		String name = SanitateResourceName(nameIn);
		if (!isRouting_)
		{
			isRouting_ = true;
			for (unsigned i = 0; i < resourceRouters_.Size(); ++i)
				resourceRouters_[i]->Route(name, RESOURCE_GETFILE);
			isRouting_ = false;
		}

		if (name.Length())
		{
			File* file = 0;

			if (searchPackagesFirst_)
			{
				file = SearchPackages(name);
				if (!file)
					file = SearchResourceDirs(name);
			}
			else
			{
				file = SearchResourceDirs(name);
				if (!file)
					file = SearchPackages(name);
			}

			if (file)
				return SharedPtr<File>(file);
		}

		if (sendEventOnFailure)
		{
			if (resourceRouters_.Size() && name.Empty() && !nameIn.Empty())
				SAPPHIRE_LOGERROR("Resource request " + nameIn + " was blocked");
			else
				SAPPHIRE_LOGERROR("Could not find resource " + name);

			if (Thread::IsMainThread())
			{
				using namespace ResourceNotFound;

				VariantMap& eventData = GetEventDataMap();
				eventData[P_RESOURCENAME] = name.Length() ? name : nameIn;
				SendEvent(E_RESOURCENOTFOUND, eventData);
			}
		}

		return SharedPtr<File>();
	}

	Resource* ResourceCache::GetExistingResource(StringHash type, const String& nameIn)
	{
		String name = SanitateResourceName(nameIn);

		if (!Thread::IsMainThread())
		{
			SAPPHIRE_LOGERROR("Attempted to get resource " + name + " from outside the main thread");
			return 0;
		}

		// If empty name, return null pointer immediately
		if (name.Empty())
			return 0;

		StringHash nameHash(name);

		const SharedPtr<Resource>& existing = FindResource(type, nameHash);
		return existing;
	}

	Resource* ResourceCache::GetResource(StringHash type, const String& nameIn, bool sendEventOnFailure)
	{
		String name = SanitateResourceName(nameIn);

		if (!Thread::IsMainThread())
		{
			SAPPHIRE_LOGERROR("Attempted to get resource " + name + " from outside the main thread");
			return 0;
		}

		// If empty name, return null pointer immediately
		if (name.Empty())
			return 0;

		StringHash nameHash(name);

#ifdef SAPPHIRE_THREADING
		// Check if the resource is being background loaded but is now needed immediately
		backgroundLoader_->WaitForResource(type, nameHash);
#endif

		const SharedPtr<Resource>& existing = FindResource(type, nameHash);
		if (existing)
			return existing;

		SharedPtr<Resource> resource;
		// Make sure the pointer is non-null and is a Resource subclass
		resource = DynamicCast<Resource>(context_->CreateObject(type));
		if (!resource)
		{
			SAPPHIRE_LOGERROR("Could not load unknown resource type " + String(type));

			if (sendEventOnFailure)
			{
				using namespace UnknownResourceType;

				VariantMap& eventData = GetEventDataMap();
				eventData[P_RESOURCETYPE] = type;
				SendEvent(E_UNKNOWNRESOURCETYPE, eventData);
			}

			return 0;
		}

		// Attempt to load the resource
		SharedPtr<File> file = GetFile(name, sendEventOnFailure);
		if (!file)
			return 0;   // Error is already logged

		SAPPHIRE_LOGDEBUG("Loading resource " + name);
		resource->SetName(name);

		if (!resource->Load(*(file.Get())))
		{
			// Error should already been logged by corresponding resource descendant class
			if (sendEventOnFailure)
			{
				using namespace LoadFailed;

				VariantMap& eventData = GetEventDataMap();
				eventData[P_RESOURCENAME] = name;
				SendEvent(E_LOADFAILED, eventData);
			}

			if (!returnFailedResources_)
				return 0;
		}

		// Store to cache
		resource->ResetUseTimer();
		resourceGroups_[type].resources_[nameHash] = resource;
		UpdateResourceGroup(type);

		return resource;
	}

	bool ResourceCache::BackgroundLoadResource(StringHash type, const String& nameIn, bool sendEventOnFailure, Resource* caller)
	{
#ifdef SAPPHIRE_THREADING
		// If empty name, fail immediately
		String name = SanitateResourceName(nameIn);
		if (name.Empty())
			return false;

		// First check if already exists as a loaded resource
		StringHash nameHash(name);
		if (FindResource(type, nameHash) != noResource)
			return false;

		return backgroundLoader_->QueueResource(type, name, sendEventOnFailure, caller);
#else
		// When threading not supported, fall back to synchronous loading
		return GetResource(type, nameIn, sendEventOnFailure);
#endif
	}

	SharedPtr<Resource> ResourceCache::GetTempResource(StringHash type, const String& nameIn, bool sendEventOnFailure)
	{
		String name = SanitateResourceName(nameIn);

		// If empty name, return null pointer immediately
		if (name.Empty())
			return SharedPtr<Resource>();

		SharedPtr<Resource> resource;
		// Make sure the pointer is non-null and is a Resource subclass
		resource = DynamicCast<Resource>(context_->CreateObject(type));
		if (!resource)
		{
			SAPPHIRE_LOGERROR("Could not load unknown resource type " + String(type));

			if (sendEventOnFailure)
			{
				using namespace UnknownResourceType;

				VariantMap& eventData = GetEventDataMap();
				eventData[P_RESOURCETYPE] = type;
				SendEvent(E_UNKNOWNRESOURCETYPE, eventData);
			}

			return SharedPtr<Resource>();
		}

		// Attempt to load the resource
		SharedPtr<File> file = GetFile(name, sendEventOnFailure);
		if (!file)
			return SharedPtr<Resource>();  // Error is already logged

		SAPPHIRE_LOGDEBUG("Loading temporary resource " + name);
		resource->SetName(file->GetName());

		if (!resource->Load(*(file.Get())))
		{
			// Error should already been logged by corresponding resource descendant class
			if (sendEventOnFailure)
			{
				using namespace LoadFailed;

				VariantMap& eventData = GetEventDataMap();
				eventData[P_RESOURCENAME] = name;
				SendEvent(E_LOADFAILED, eventData);
			}

			return SharedPtr<Resource>();
		}

		return resource;
	}

	unsigned ResourceCache::GetNumBackgroundLoadResources() const
	{
#ifdef SAPPHIRE_THREADING
		return backgroundLoader_->GetNumQueuedResources();
#else
		return 0;
#endif
	}

	void ResourceCache::GetResources(PODVector<Resource*>& result, StringHash type) const
	{
		result.Clear();
		HashMap<StringHash, ResourceGroup>::ConstIterator i = resourceGroups_.Find(type);
		if (i != resourceGroups_.End())
		{
			for (HashMap<StringHash, SharedPtr<Resource> >::ConstIterator j = i->second_.resources_.Begin();
				j != i->second_.resources_.End(); ++j)
				result.Push(j->second_);
		}
	}

	bool ResourceCache::Exists(const String& nameIn) const
	{
		MutexLock lock(resourceMutex_);

		String name = SanitateResourceName(nameIn);
		if (!isRouting_)
		{
			isRouting_ = true;
			for (unsigned i = 0; i < resourceRouters_.Size(); ++i)
				resourceRouters_[i]->Route(name, RESOURCE_CHECKEXISTS);
			isRouting_ = false;
		}

		if (name.Empty())
			return false;

		for (unsigned i = 0; i < packages_.Size(); ++i)
		{
			if (packages_[i]->Exists(name))
				return true;
		}

		FileSystem* fileSystem = GetSubsystem<FileSystem>();
		for (unsigned i = 0; i < resourceDirs_.Size(); ++i)
		{
			if (fileSystem->FileExists(resourceDirs_[i] + name))
				return true;
		}

		// Fallback using absolute path
		return fileSystem->FileExists(name);
	}

	unsigned long long ResourceCache::GetMemoryBudget(StringHash type) const
	{
		HashMap<StringHash, ResourceGroup>::ConstIterator i = resourceGroups_.Find(type);
		return i != resourceGroups_.End() ? i->second_.memoryBudget_ : 0;
	}

	unsigned long long ResourceCache::GetMemoryUse(StringHash type) const
	{
		HashMap<StringHash, ResourceGroup>::ConstIterator i = resourceGroups_.Find(type);
		return i != resourceGroups_.End() ? i->second_.memoryUse_ : 0;
	}

	unsigned long long ResourceCache::GetTotalMemoryUse() const
	{
		unsigned long long total = 0;
		for (HashMap<StringHash, ResourceGroup>::ConstIterator i = resourceGroups_.Begin(); i != resourceGroups_.End(); ++i)
			total += i->second_.memoryUse_;
		return total;
	}

	String ResourceCache::GetResourceFileName(const String& name) const
	{
		MutexLock lock(resourceMutex_);

		FileSystem* fileSystem = GetSubsystem<FileSystem>();
		for (unsigned i = 0; i < resourceDirs_.Size(); ++i)
		{
			if (fileSystem->FileExists(resourceDirs_[i] + name))
				return resourceDirs_[i] + name;
		}

		return String();
	}

	ResourceRouter* ResourceCache::GetResourceRouter(unsigned index) const
	{
		return index < resourceRouters_.Size() ? resourceRouters_[index] : (ResourceRouter*)0;
	}

	String ResourceCache::GetPreferredResourceDir(const String& path) const
	{
		String fixedPath = AddTrailingSlash(path);

		bool pathHasKnownDirs = false;
		bool parentHasKnownDirs = false;

		FileSystem* fileSystem = GetSubsystem<FileSystem>();

		for (unsigned i = 0; checkDirs[i] != 0; ++i)
		{
			if (fileSystem->DirExists(fixedPath + checkDirs[i]))
			{
				pathHasKnownDirs = true;
				break;
			}
		}
		if (!pathHasKnownDirs)
		{
			String parentPath = GetParentPath(fixedPath);
			for (unsigned i = 0; checkDirs[i] != 0; ++i)
			{
				if (fileSystem->DirExists(parentPath + checkDirs[i]))
				{
					parentHasKnownDirs = true;
					break;
				}
			}
			// If path does not have known subdirectories, but the parent path has, use the parent instead
			if (parentHasKnownDirs)
				fixedPath = parentPath;
		}

		return fixedPath;
	}

	String ResourceCache::SanitateResourceName(const String& nameIn) const
	{
		// Sanitate unsupported constructs from the resource name
		String name = GetInternalPath(nameIn);
		name.Replace("../", "");
		name.Replace("./", "");

		// If the path refers to one of the resource directories, normalize the resource name
		FileSystem* fileSystem = GetSubsystem<FileSystem>();
		if (resourceDirs_.Size())
		{
			String namePath = GetPath(name);
			String exePath = fileSystem->GetProgramDir();
			for (unsigned i = 0; i < resourceDirs_.Size(); ++i)
			{
				String relativeResourcePath = resourceDirs_[i];
				if (relativeResourcePath.StartsWith(exePath))
					relativeResourcePath = relativeResourcePath.Substring(exePath.Length());

				if (namePath.StartsWith(resourceDirs_[i], false))
					namePath = namePath.Substring(resourceDirs_[i].Length());
				else if (namePath.StartsWith(relativeResourcePath, false))
					namePath = namePath.Substring(relativeResourcePath.Length());
			}

			name = namePath + GetFileNameAndExtension(name);
		}

		return name.Trimmed();
	}

	String ResourceCache::SanitateResourceDirName(const String& nameIn) const
	{
		String fixedPath = AddTrailingSlash(nameIn);
		if (!IsAbsolutePath(fixedPath))
			fixedPath = GetSubsystem<FileSystem>()->GetCurrentDir() + fixedPath;

		// Sanitate away /./ construct
		fixedPath.Replace("/./", "/");

		return fixedPath.Trimmed();
	}

	void ResourceCache::StoreResourceDependency(Resource* resource, const String& dependency)
	{
		if (!resource)
			return;

		MutexLock lock(resourceMutex_);

		StringHash nameHash(resource->GetName());
		HashSet<StringHash>& dependents = dependentResources_[dependency];
		dependents.Insert(nameHash);
	}

	void ResourceCache::ResetDependencies(Resource* resource)
	{
		if (!resource)
			return;

		MutexLock lock(resourceMutex_);

		StringHash nameHash(resource->GetName());

		for (HashMap<StringHash, HashSet<StringHash> >::Iterator i = dependentResources_.Begin(); i != dependentResources_.End();)
		{
			HashSet<StringHash>& dependents = i->second_;
			dependents.Erase(nameHash);
			if (dependents.Empty())
				i = dependentResources_.Erase(i);
			else
				++i;
		}
	}

	String ResourceCache::PrintMemoryUsage() const
	{
		String output = "Resource Type                 Cnt       Avg       Max    Budget     Total\n\n";
		char outputLine[256];

		unsigned totalResourceCt = 0;
		unsigned long long totalLargest = 0;
		unsigned long long totalAverage = 0;
		unsigned long long totalUse = GetTotalMemoryUse();

		for (HashMap<StringHash, ResourceGroup>::ConstIterator cit = resourceGroups_.Begin(); cit != resourceGroups_.End(); ++cit)
		{
			const unsigned resourceCt = cit->second_.resources_.Size();
			unsigned long long average = 0;
			if (resourceCt > 0)
				average = cit->second_.memoryUse_ / resourceCt;
			else
				average = 0;
			unsigned long long largest = 0;
			for (HashMap<StringHash, SharedPtr<Resource> >::ConstIterator resIt = cit->second_.resources_.Begin(); resIt != cit->second_.resources_.End(); ++resIt)
			{
				if (resIt->second_->GetMemoryUse() > largest)
					largest = resIt->second_->GetMemoryUse();
				if (largest > totalLargest)
					totalLargest = largest;
			}

			totalResourceCt += resourceCt;

			const String countString(cit->second_.resources_.Size());
			const String memUseString = GetFileSizeString(average);
			const String memMaxString = GetFileSizeString(largest);
			const String memBudgetString = GetFileSizeString(cit->second_.memoryBudget_);
			const String memTotalString = GetFileSizeString(cit->second_.memoryUse_);
			const String resTypeName = context_->GetTypeName(cit->first_);

			memset(outputLine, ' ', 256);
			outputLine[255] = 0;
			sprintf(outputLine, "%-28s %4s %9s %9s %9s %9s\n", resTypeName.CString(), countString.CString(), memUseString.CString(), memMaxString.CString(), memBudgetString.CString(), memTotalString.CString());

			output += ((const char*)outputLine);
		}

		if (totalResourceCt > 0)
			totalAverage = totalUse / totalResourceCt;

		const String countString(totalResourceCt);
		const String memUseString = GetFileSizeString(totalAverage);
		const String memMaxString = GetFileSizeString(totalLargest);
		const String memTotalString = GetFileSizeString(totalUse);

		memset(outputLine, ' ', 256);
		outputLine[255] = 0;
		sprintf(outputLine, "%-28s %4s %9s %9s %9s %9s\n", "All", countString.CString(), memUseString.CString(), memMaxString.CString(), "-", memTotalString.CString());
		output += ((const char*)outputLine);

		return output;
	}

	const SharedPtr<Resource>& ResourceCache::FindResource(StringHash type, StringHash nameHash)
	{
		MutexLock lock(resourceMutex_);

		HashMap<StringHash, ResourceGroup>::Iterator i = resourceGroups_.Find(type);
		if (i == resourceGroups_.End())
			return noResource;
		HashMap<StringHash, SharedPtr<Resource> >::Iterator j = i->second_.resources_.Find(nameHash);
		if (j == i->second_.resources_.End())
			return noResource;

		return j->second_;
	}

	const SharedPtr<Resource>& ResourceCache::FindResource(StringHash nameHash)
	{
		MutexLock lock(resourceMutex_);

		for (HashMap<StringHash, ResourceGroup>::Iterator i = resourceGroups_.Begin(); i != resourceGroups_.End(); ++i)
		{
			HashMap<StringHash, SharedPtr<Resource> >::Iterator j = i->second_.resources_.Find(nameHash);
			if (j != i->second_.resources_.End())
				return j->second_;
		}

		return noResource;
	}

	void ResourceCache::ReleasePackageResources(PackageFile* package, bool force)
	{
		HashSet<StringHash> affectedGroups;

		const HashMap<String, PackageEntry>& entries = package->GetEntries();
		for (HashMap<String, PackageEntry>::ConstIterator i = entries.Begin(); i != entries.End(); ++i)
		{
			StringHash nameHash(i->first_);

			// We do not know the actual resource type, so search all type containers
			for (HashMap<StringHash, ResourceGroup>::Iterator j = resourceGroups_.Begin(); j != resourceGroups_.End(); ++j)
			{
				HashMap<StringHash, SharedPtr<Resource> >::Iterator k = j->second_.resources_.Find(nameHash);
				if (k != j->second_.resources_.End())
				{
					// If other references exist, do not release, unless forced
					if ((k->second_.Refs() == 1 && k->second_.WeakRefs() == 0) || force)
					{
						j->second_.resources_.Erase(k);
						affectedGroups.Insert(j->first_);
					}
					break;
				}
			}
		}

		for (HashSet<StringHash>::Iterator i = affectedGroups.Begin(); i != affectedGroups.End(); ++i)
			UpdateResourceGroup(*i);
	}

	void ResourceCache::UpdateResourceGroup(StringHash type)
	{
		HashMap<StringHash, ResourceGroup>::Iterator i = resourceGroups_.Find(type);
		if (i == resourceGroups_.End())
			return;

		for (;;)
		{
			unsigned totalSize = 0;
			unsigned oldestTimer = 0;
			HashMap<StringHash, SharedPtr<Resource> >::Iterator oldestResource = i->second_.resources_.End();

			for (HashMap<StringHash, SharedPtr<Resource> >::Iterator j = i->second_.resources_.Begin();
				j != i->second_.resources_.End(); ++j)
			{
				totalSize += j->second_->GetMemoryUse();
				unsigned useTimer = j->second_->GetUseTimer();
				if (useTimer > oldestTimer)
				{
					oldestTimer = useTimer;
					oldestResource = j;
				}
			}

			i->second_.memoryUse_ = totalSize;

			// If memory budget defined and is exceeded, remove the oldest resource and loop again
			// (resources in use always return a zero timer and can not be removed)
			if (i->second_.memoryBudget_ && i->second_.memoryUse_ > i->second_.memoryBudget_ &&
				oldestResource != i->second_.resources_.End())
			{
				SAPPHIRE_LOGDEBUG("Resource group " + oldestResource->second_->GetTypeName() + " over memory budget, releasing resource " +
					oldestResource->second_->GetName());
				i->second_.resources_.Erase(oldestResource);
			}
			else
				break;
		}
	}

	void ResourceCache::HandleBeginFrame(StringHash eventType, VariantMap& eventData)
	{
		for (unsigned i = 0; i < fileWatchers_.Size(); ++i)
		{
			String fileName;
			while (fileWatchers_[i]->GetNextChange(fileName))
			{
				ReloadResourceWithDependencies(fileName);

				// Finally send a general file changed event even if the file was not a tracked resource
				using namespace FileChanged;

				VariantMap& eventData = GetEventDataMap();
				eventData[P_FILENAME] = fileWatchers_[i]->GetPath() + fileName;
				eventData[P_RESOURCENAME] = fileName;
				SendEvent(E_FILECHANGED, eventData);
			}
		}

		// Check for background loaded resources that can be finished
#ifdef SAPPHIRE_THREADING
	{
		SAPPHIRE_PROFILE(FinishBackgroundResources);
		backgroundLoader_->FinishResources(finishBackgroundResourcesMs_);
	}
#endif
	}

	File* ResourceCache::SearchResourceDirs(const String& nameIn)
	{
		FileSystem* fileSystem = GetSubsystem<FileSystem>();
		for (unsigned i = 0; i < resourceDirs_.Size(); ++i)
		{
			if (fileSystem->FileExists(resourceDirs_[i] + nameIn))
			{
				// Construct the file first with full path, then rename it to not contain the resource path,
				// so that the file's name can be used in further GetFile() calls (for example over the network)
				File* file(new File(context_, resourceDirs_[i] + nameIn));
				file->SetName(nameIn);
				return file;
			}
		}

		// Fallback using absolute path
		if (fileSystem->FileExists(nameIn))
			return new File(context_, nameIn);

		return 0;
	}

	File* ResourceCache::SearchPackages(const String& nameIn)
	{
		for (unsigned i = 0; i < packages_.Size(); ++i)
		{
			if (packages_[i]->Exists(nameIn))
				return new File(context_, packages_[i], nameIn);
		}

		return 0;
	}

	void RegisterResourceLibrary(Context* context)
	{
		//ע��Image���󹤳�
		Image::RegisterObject(context);
		//ע��Json���󹤳�
		JSONFile::RegisterObject(context);
		//ע��PListFile���󹤳�
		PListFile::RegisterObject(context);
		//ע��XMLFile���󹤳�
		XMLFile::RegisterObject(context);
	}

}