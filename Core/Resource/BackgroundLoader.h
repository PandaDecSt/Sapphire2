#pragma once

#include "../Container/HashMap.h"
#include "../Container/HashSet.h"
#include "../Core/Mutex.h"
#include "../Container/Ptr.h"
#include "../Container/RefCounted.h"
#include "../Core/Thread.h"
#include "../Math/StringHash.h"

namespace Sapphire
{

	class Resource;
	class ResourceCache;

	/// ��̨����һ����Դ�Ķ�����Ŀ
	struct BackgroundLoadItem
	{
		/// ��Դָ��.
		SharedPtr<Resource> resource_;
		/// �����Դ��������Դ
		HashSet<Pair<StringHash, StringHash> > dependencies_;
		/// ���������Դ����Դ
		HashSet<Pair<StringHash, StringHash> > dependents_;
		/// �Ƿ���ʧ���¼�
		bool sendEventOnFailure_;
	};

	/// ��Դ��̨��������������Դ����ResourceCache.
	class BackgroundLoader : public RefCounted, public Thread
	{
	public:
		 
		BackgroundLoader(ResourceCache* owner);

		/// ��Դ��̨����ѭ��
		virtual void ThreadFunction();

		/// ���л�һ����Դ���ء� ���ֱ���ȷ��һ�µĸ�ʽ��������л�����true (û���ظ�������Դʱ��֪������)
		bool QueueResource(StringHash type, const String& name, bool sendEventOnFailure, Resource* caller);
		///  �ȴ�����ɴӻ�����������ص���Դ
		void WaitForResource(StringHash type, StringHash nameHash);
		/// ����׼����ɵ���Դ
		void FinishResources(int maxMs);

		/// �����ڼ��ض��е���Դ����
		unsigned GetNumQueuedResources() const;

	private:
		/// �ս�һ����̨������Դ��
		void FinishBackgroundLoading(BackgroundLoadItem& item);

		/// ��Դ����
		ResourceCache* owner_;
		/// ���ʺ�̨���ض��е��̰߳�ȫ������
		mutable Mutex backgroundLoadMutex_;
		/// ��̨���صĶ��л�����Դ
		// pair<��Դ���ͣ���Դ��>
		HashMap<Pair<StringHash, StringHash>, BackgroundLoadItem> backgroundLoadQueue_;
	};
}