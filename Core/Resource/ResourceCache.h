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
		/// ��̨����һ����Դ�� �����ʱ����һ���¼��� ����ɹ�����true�����浽���ض��С�    ����Ѿ����ڷ���false�� ���Ա������̵߳���
		bool BackgroundLoadResource(StringHash type, const String& name, bool sendEventOnFailure = true, Resource* caller = 0);
		/// ���غ�̨Ҫ���ص���Դ��
		unsigned GetNumBackgroundLoadResources() const;
		/// �������м��ص��ض����͵���Դ
		void GetResources(PODVector<Resource*>& result, StringHash type) const;
		/// ����һ���ض����ͺ����ֵ��Ѽ�����Դ�����û�ҵ����ؿ�
		Resource* GetExistingResource(StringHash type, const String& name);

		/// ���������Ѽ�����Դ
		const HashMap<StringHash, ResourceGroup>& GetAllResources() const { return resourceGroups_; }

		/// ����������ӵ���Դ����Ŀ¼
		const Vector<String>& GetResourceDirs() const { return resourceDirs_; }

		/// ����������ӵİ��ļ�
		const Vector<SharedPtr<PackageFile> >& GetPackageFiles() const { return packages_; }

		/// ��ȡģ�����͵���Դ
		template <class T> T* GetResource(const String& name, bool sendEventOnFailure = true);
		/// ��ȡһ���Ѵ��ڵ���Դ
		template <class T> T* GetExistingResource(const String& name);
		/// ��ȡû�б����ڻ����е���Դ
		template <class T> SharedPtr<T> GetTempResource(const String& name, bool sendEventOnFailure = true);
		/// ��̨������Դ��ģ�廯�汾
		template <class T> bool BackgroundLoadResource(const String& name, bool sendEventOnFailure = true, Resource* caller = 0);
		/// �����ض�������Դ��ģ�廯�汾
		template <class T> void GetResources(PODVector<T*>& result) const;
		/// ����һ���ļ��Ƿ����
		bool Exists(const String& name) const;
		/// ����һ����Դ���͵��ڴ�Ԥ��
		unsigned long long GetMemoryBudget(StringHash type) const;
		/// ����һ����Դ���͵����ڴ�ռ��
		unsigned long long GetMemoryUse(StringHash type) const;
		/// ����������Դռ�õ����ڴ�
		unsigned long long GetTotalMemoryUse() const;
		/// ������Է����ļ����ľ���·��
		String GetResourceFileName(const String& name) const;

		/// �����Զ�������Դ�Ƿ��
		bool GetAutoReloadResources() const { return autoReloadResources_; }

		/// �����Ƿ񷵻ؼ���ʧ�ܵ���Դ
		bool GetReturnFailedResources() const { return returnFailedResources_; }

		/// �����Ƿ�����������
		bool GetSearchPackagesFirst() const { return searchPackagesFirst_; }

		/// ���ػ��ں�̨������Դ����������
		int GetFinishBackgroundResourcesMs() const { return finishBackgroundResourcesMs_; }

		/// ȡ��һ����Դrouter
		ResourceRouter* GetResourceRouter(unsigned index) const;

		/// ����·�������ĸ�·����֯����Դ��Ŀ¼
		String GetPreferredResourceDir(const String& path) const;
		/// ����Դ���Ƴ���֧�ֵĹ��캯������������ԡ�����������Ա�׼�������ļ�������Դ·�������·��
		String SanitateResourceName(const String& name) const;
		/// ��һ����ԴĿ¼���Ƴ�����Ҫ�Ĺ���������ȷ������Ϊһ������·��
		String SanitateResourceDirName(const String& name) const;
		/// ����һ����Դ�������� ��������ļ��ı䣬 �����Դ��������
		void StoreResourceDependency(Resource* resource, const String& dependency);
		/// ����һ����Դ�������Ĺ�ϵ
		void ResetDependencies(Resource* resource);

		/// ����һ�������ڴ�ʹ��״̬�ĸ�ʽ�����ַ���
		String PrintMemoryUsage() const;

	private:
		/// ������Դ
		const SharedPtr<Resource>& FindResource(StringHash type, StringHash nameHash);
		/// ֻͨ�����ֲ�����Դ���������е�������
		const SharedPtr<Resource>& FindResource(StringHash nameHash);
		/// ��һ�����ļ��ͷż��ص���Դ
		void ReleasePackageResources(PackageFile* package, bool force = false);
		/// ����һ����Դ�顣 ��������ڴ�Ԥ�㣬���¼����ڴ�ʹ�������ͷ���Դ
		void UpdateResourceGroup(StringHash type);
		/// ����begin frame�¼�. ��Դ�Զ����ز��ҽ�����̨������Դ�Ĵ���
		void HandleBeginFrame(StringHash eventType, VariantMap& eventData);
		/// ��Ŀ¼�����ļ�
		File* SearchResourceDirs(const String& nameIn);
		/// �Ӱ������ļ�
		File* SearchPackages(const String& nameIn);

		/// �̰߳�ȫ������ԴĿ¼����Դ������Դ�����Ļ�����
		mutable Mutex resourceMutex_;
		/// ��Դ����
		HashMap<StringHash, ResourceGroup> resourceGroups_;
		/// ��Դ����Ŀ¼
		Vector<String> resourceDirs_;
		/// ��ԴĿ¼���ļ��۲��ߣ�������Զ����صĻ�
		Vector<SharedPtr<FileWatcher> > fileWatchers_;
		/// ���ļ�
		Vector<SharedPtr<PackageFile> > packages_;
		/// ��Դ������ ֻ�����Զ����ء� 
		HashMap<StringHash, HashSet<StringHash> > dependentResources_;
		/// ��̨��Դ������
		SharedPtr<BackgroundLoader> backgroundLoader_;
		/// ��ԴRouter
		Vector<SharedPtr<ResourceRouter> > resourceRouters_;
		/// ��Դ�Զ����ر�־
		bool autoReloadResources_;
		/// ����ʧ�ܵ���Դ��־
		bool returnFailedResources_;
		/// �������ȼ���־
		bool searchPackagesFirst_;
		/// ��Դrouting��־���������޵ݹ�
		mutable bool isRouting_;
		/// ÿ֡���ں�̨������Դ����������
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
		//ת����Դ����
		PODVector<Resource*>& resources = reinterpret_cast<PODVector<Resource*>&>(result);
		//��ȡ������Ϣ
		StringHash type = T::GetTypeStatic();
		//��ȡ��Դ
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
