#pragma once

#include "../Container/HashSet.h"
#include "../Container/List.h"
#include "../Core/Mutex.h"
#include "../IO/File.h"
#include "../Resource/Resource.h"

namespace Sapphire
{
	class BackgroundLoader;
	class FileWatcher;
	class PackageFile;

	// �������ȼ��Ա㽫һ�������ļ��Ƶ������ĩβ
	static const unsigned PRIORITY_LAST = 0xffffffff;

	/// �ض����͵���Դ����
	struct ResourceGroup
	{
		ResourceGroup() :
			memoryBudget_(0),
			memoryUse_(0)
		{
		}

		/// �ڴ�Ԥ��
		unsigned long long memoryBudget_;
		/// ��ǰ�ڴ�ʹ����
		unsigned long long memoryUse_;
		/// ��Դ.
		HashMap<StringHash, SharedPtr<Resource> > resources_;
	};

	/// ��Դ��������
	enum ResourceRequest
	{
		RESOURCE_CHECKEXISTS = 0,   //������
		RESOURCE_GETFILE = 1       //��ȡ�ļ�
	};

	/// ��ѡ��Դ�������������Ծܾ��������²�ѯ��Դ�ļ������ߴ������������
	class SAPPHIRE_API ResourceRouter : public Object
	{
	public:
		 
		ResourceRouter(Context* context) :
			Object(context)
		{
		}

		/// ������Դ�������޸���Դ���ַ���������Ϊ�յ��ַ�����ζ����Դû�ҵ����ܱ�����
		virtual void Route(String& name, ResourceRequest requestType) = 0;
	};


	/// ��Դ������ϵͳ��  ����Ҫ��ʱ�������Դ���ұ��������Ա��Ժ����
	class SAPPHIRE_API ResourceCache : public Object
	{
		SAPPHIRE_OBJECT(ResourceCache, Object);

	public:
		/// ������
		ResourceCache(Context* context);
		/// �������ͷ�������Դ
		virtual ~ResourceCache();

		/// ���һ����Դ����Ŀ¼�� ��ѡ���ȼ�������������������˳��
		bool AddResourceDir(const String& pathName, unsigned priority = PRIORITY_LAST);
		/// Ϊ������Դ���һ�����ļ��� ��ѡ���ȼ�������������������˳��
		bool AddPackageFile(PackageFile* package, unsigned priority = PRIORITY_LAST);
		///  �����һ�����ļ�������Դ����ѡ���ȼ�������������������˳��
		bool AddPackageFile(const String& fileName, unsigned priority = PRIORITY_LAST);
		/// ���һ���ֶ���������Դ�� ������Ψһ������
		bool AddManualResource(Resource* resource);
		/// �Ƴ�һ����Դ����Ŀ¼
		void RemoveResourceDir(const String& pathName);
		/// �Ƴ�һ�����ļ��� ��ѡ�ͷ��������ص���Դ
		void RemovePackageFile(PackageFile* package, bool releaseResources = true, bool forceRelease = false);
		/// �Ƴ�һ�����ļ��� ��ѡ�ͷ��������ص���Դ
		void RemovePackageFile(const String& fileName, bool releaseResources = true, bool forceRelease = false);
		/// ͨ�������ͷ�һ����Դ
		void ReleaseResource(StringHash type, const String& name, bool force = false);
		/// �ͷ�����ָ�����͵���Դ
		void ReleaseResources(StringHash type, bool force = false);
		/// �ͷ�һ���ض����ͺͲ������ֵ���Դ
		void ReleaseResources(StringHash type, const String& partialName, bool force = false);
		/// �ͷŲ������ֵ�������Դ
		void ReleaseResources(const String& partialName, bool force = false);
		/// �ͷ�������Դ�� ��force ��־ ==falseʱ�� �ͷ����е�ǰ���õ���Դ
		void ReleaseAllResources(bool force = false);
		/// ������һ����Դ�� �ɹ�����true��ʧ�ܵ�����²���ӻ������Ƴ���Դ
		bool ReloadResource(Resource* resource);
		/// ͨ��һ����������һ����Դ��  �����Ҫ������������������Դ
		void ReloadResourceWithDependencies(const String& fileName);
		/// Ϊָ������Դ���������ڴ�Ԥ���� 0��ָ������
		void SetMemoryBudget(StringHash type, unsigned long long budget);
		/// �򿪻�رյ��ʼ����޸�ʱ�Զ�������Դ�� Ĭ�Ϲر�
		void SetAutoReloadResources(bool enable);
		/// �򿪻�رռ���ʧ��ʱ������Դ�� Ĭ��false �� ��Ա༭��Դ����ʧ������������
		void SetReturnFailedResources(bool enable) { returnFailedResources_ = enable; }

		/// ���嵱��ȡ��Դʱ���ȼ����ļ�����Ŀ¼�� true���ڰ���false����Ŀ¼
		void SetSearchPackagesFirst(bool value) { searchPackagesFirst_ = value; }

		/// ���ö���ÿ֡�����ٺ��뻨����ɺ�̨������Դ��
		void SetFinishBackgroundResourcesMs(int ms) { finishBackgroundResourcesMs_ = Max(ms, 1); }

		/// �����ԴRouter���� Ĭ��Ϊ�գ� ��������routing����
		void AddResourceRouter(ResourceRouter* router, bool addAsFirst = false);
		/// �Ƴ�һ����Դrouter����
		void RemoveResourceRouter(ResourceRouter* router);

		///����Դ����·������ļ���򿪲�����һ���ļ������û���ҵ��� �þ���·������������ ʧ�ܷ���false
		SharedPtr<File> GetFile(const String& name, bool sendEventOnFailure = true);
		/// ͨ�����ͺ�����ȡ����Դ�� ���δ����������ء����û���ҵ���ʧ�ܷ���null. ���ǵ��ù�SetReturnFailedResources(true)��ֻ�ܱ����̵߳���
		Resource* GetResource(StringHash type, const String& name, bool sendEventOnFailure = true);
		///  ����һ����Դ�������浽��Դ�����У������Դ�ܰ�ȫ����ȫ���أ���ô���Ա������̵߳��á�(���磺GPU���ݾͲ���)
		SharedPtr<Resource> GetTempResource(StringHash type, const String& name, bool sendEventOnFailure = true);
		/// Background load a resource. An event will be sent when complete. Return true if successfully stored to the load queue, false if eg. already exists. Can be called from outside the main thread.
		bool BackgroundLoadResource(StringHash type, const String& name, bool sendEventOnFailure = true, Resource* caller = 0);
		/// Return number of pending background-loaded resources.
		unsigned GetNumBackgroundLoadResources() const;
		/// Return all loaded resources of a specific type.
		void GetResources(PODVector<Resource*>& result, StringHash type) const;
		/// Return an already loaded resource of specific type & name, or null if not found. Will not load if does not exist.
		Resource* GetExistingResource(StringHash type, const String& name);

		/// Return all loaded resources.
		const HashMap<StringHash, ResourceGroup>& GetAllResources() const { return resourceGroups_; }

		/// Return added resource load directories.
		const Vector<String>& GetResourceDirs() const { return resourceDirs_; }

		/// Return added package files.
		const Vector<SharedPtr<PackageFile> >& GetPackageFiles() const { return packages_; }

		/// Template version of returning a resource by name.
		template <class T> T* GetResource(const String& name, bool sendEventOnFailure = true);
		/// Template version of returning an existing resource by name.
		template <class T> T* GetExistingResource(const String& name);
		/// Template version of loading a resource without storing it to the cache.
		template <class T> SharedPtr<T> GetTempResource(const String& name, bool sendEventOnFailure = true);
		/// Template version of queueing a resource background load.
		template <class T> bool BackgroundLoadResource(const String& name, bool sendEventOnFailure = true, Resource* caller = 0);
		/// Template version of returning loaded resources of a specific type.
		template <class T> void GetResources(PODVector<T*>& result) const;
		/// Return whether a file exists by name.
		bool Exists(const String& name) const;
		/// Return memory budget for a resource type.
		unsigned long long GetMemoryBudget(StringHash type) const;
		/// Return total memory use for a resource type.
		unsigned long long GetMemoryUse(StringHash type) const;
		/// Return total memory use for all resources.
		unsigned long long GetTotalMemoryUse() const;
		/// Return full absolute file name of resource if possible.
		String GetResourceFileName(const String& name) const;

		/// Return whether automatic resource reloading is enabled.
		bool GetAutoReloadResources() const { return autoReloadResources_; }

		/// Return whether resources that failed to load are returned.
		bool GetReturnFailedResources() const { return returnFailedResources_; }

		/// Return whether when getting resources should check package files or directories first.
		bool GetSearchPackagesFirst() const { return searchPackagesFirst_; }

		/// Return how many milliseconds maximum to spend on finishing background loaded resources.
		int GetFinishBackgroundResourcesMs() const { return finishBackgroundResourcesMs_; }

		/// Return a resource router by index.
		ResourceRouter* GetResourceRouter(unsigned index) const;

		/// Return either the path itself or its parent, based on which of them has recognized resource subdirectories.
		String GetPreferredResourceDir(const String& path) const;
		/// Remove unsupported constructs from the resource name to prevent ambiguity, and normalize absolute filename to resource path relative if possible.
		String SanitateResourceName(const String& name) const;
		/// Remove unnecessary constructs from a resource directory name and ensure it to be an absolute path.
		String SanitateResourceDirName(const String& name) const;
		/// Store a dependency for a resource. If a dependency file changes, the resource will be reloaded.
		void StoreResourceDependency(Resource* resource, const String& dependency);
		/// Reset dependencies for a resource.
		void ResetDependencies(Resource* resource);

		/// Returns a formatted string containing the memory actively used.
		String PrintMemoryUsage() const;

	private:
		/// Find a resource.
		const SharedPtr<Resource>& FindResource(StringHash type, StringHash nameHash);
		/// Find a resource by name only. Searches all type groups.
		const SharedPtr<Resource>& FindResource(StringHash nameHash);
		/// Release resources loaded from a package file.
		void ReleasePackageResources(PackageFile* package, bool force = false);
		/// Update a resource group. Recalculate memory use and release resources if over memory budget.
		void UpdateResourceGroup(StringHash type);
		/// Handle begin frame event. Automatic resource reloads and the finalization of background loaded resources are processed here.
		void HandleBeginFrame(StringHash eventType, VariantMap& eventData);
		/// Search FileSystem for file.
		File* SearchResourceDirs(const String& nameIn);
		/// Search resource packages for file.
		File* SearchPackages(const String& nameIn);

		/// Mutex for thread-safe access to the resource directories, resource packages and resource dependencies.
		mutable Mutex resourceMutex_;
		/// Resources by type.
		HashMap<StringHash, ResourceGroup> resourceGroups_;
		/// Resource load directories.
		Vector<String> resourceDirs_;
		/// File watchers for resource directories, if automatic reloading enabled.
		Vector<SharedPtr<FileWatcher> > fileWatchers_;
		/// Package files.
		Vector<SharedPtr<PackageFile> > packages_;
		/// Dependent resources. Only used with automatic reload to eg. trigger reload of a cube texture when any of its faces change.
		HashMap<StringHash, HashSet<StringHash> > dependentResources_;
		/// Resource background loader.
		SharedPtr<BackgroundLoader> backgroundLoader_;
		/// Resource routers.
		Vector<SharedPtr<ResourceRouter> > resourceRouters_;
		/// Automatic resource reloading flag.
		bool autoReloadResources_;
		/// Return failed resources flag.
		bool returnFailedResources_;
		/// Search priority flag.
		bool searchPackagesFirst_;
		/// Resource routing flag to prevent endless recursion.
		mutable bool isRouting_;
		/// How many milliseconds maximum per frame to spend on finishing background loaded resources.
		int finishBackgroundResourcesMs_;
	};

	template <class T> T* ResourceCache::GetExistingResource(const String& name)
	{
		StringHash type = T::GetTypeStatic();
		return static_cast<T*>(GetExistingResource(type, name));
	}

	template <class T> T* ResourceCache::GetResource(const String& name, bool sendEventOnFailure)
	{
		StringHash type = T::GetTypeStatic();
		return static_cast<T*>(GetResource(type, name, sendEventOnFailure));
	}

	template <class T> SharedPtr<T> ResourceCache::GetTempResource(const String& name, bool sendEventOnFailure)
	{
		StringHash type = T::GetTypeStatic();
		return StaticCast<T>(GetTempResource(type, name, sendEventOnFailure));
	}

	template <class T> bool ResourceCache::BackgroundLoadResource(const String& name, bool sendEventOnFailure, Resource* caller)
	{
		StringHash type = T::GetTypeStatic();
		return BackgroundLoadResource(type, name, sendEventOnFailure, caller);
	}

	template <class T> void ResourceCache::GetResources(PODVector<T*>& result) const
	{
		PODVector<Resource*>& resources = reinterpret_cast<PODVector<Resource*>&>(result);
		StringHash type = T::GetTypeStatic();
		GetResources(resources, type);

		for (unsigned i = 0; i < result.Size(); ++i)
		{
			Resource* resource = resources[i];
			result[i] = static_cast<T*>(resource);
		}
	}

	/// ע����Դ����ϵͳ�Ͷ���
	void SAPPHIRE_API RegisterResourceLibrary(Context* context);

}
