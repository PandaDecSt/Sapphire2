
#include "../../Sapphire/Predefined.h"

#ifdef SAPPHIRE_THREADING



#include "../Core/Context.h"
#include "../Core/Profiler.h"
#include "../IO/Log.h"
#include "../Resource/BackgroundLoader.h"
#include "../Resource/ResourceCache.h"
#include "../Resource/ResourceEvents.h"

#include "../DebugNew.h"

namespace Sapphire
{

	BackgroundLoader::BackgroundLoader(ResourceCache* owner) :
		owner_(owner)
	{
	}

	void BackgroundLoader::ThreadFunction()
	{
		//ѭ������
		while (shouldRun_)
		{
			backgroundLoadMutex_.Acquire();

			// ��ѯû�б����صĶ�����Դ��
			HashMap<Pair<StringHash, StringHash>, BackgroundLoadItem>::Iterator i = backgroundLoadQueue_.Begin();
			while (i != backgroundLoadQueue_.End())
			{
				if (i->second_.resource_->GetAsyncLoadState() == ASYNC_QUEUED)
					break;
				else
					++i;
			}
			//û�ҵ�
			if (i == backgroundLoadQueue_.End())
			{
				//û����Դ��Ҫ������
				backgroundLoadMutex_.Release();
				Time::Sleep(5);
			}
			else
			{   //ȡ�øú�̨������Ŀ
				BackgroundLoadItem& item = i->second_;
				Resource* resource = item.resource_;
				// ��ȷ����Ŀδ�Ӷ������Ƴ���״̬��queue��load
				backgroundLoadMutex_.Release();

				bool success = false;
				//�Ӹü�������������Դ�����л�ȡ�ļ�
				SharedPtr<File> file = owner_->GetFile(resource->GetName(), item.sendEventOnFailure_);
				if (file)
				{
					//����״̬
					resource->SetAsyncLoadState(ASYNC_LOADING);
					//��ʼ�����ļ�
					success = resource->BeginLoad(*file);
				}

				// ���ڴ���������ϵ
				// ��������������Ҫ�Զ��м���
				Pair<StringHash, StringHash> key = MakePair(resource->GetType(), resource->GetNameHash());
				backgroundLoadMutex_.Acquire();
				if (item.dependents_.Size())
				{
					//��������Դ������������Դ
					for (HashSet<Pair<StringHash, StringHash> >::Iterator i = item.dependents_.Begin();
						i != item.dependents_.End(); ++i)
					{
						//�ں�̨���ض�����Ѱ��������
						HashMap<Pair<StringHash, StringHash>, BackgroundLoadItem>::Iterator j = backgroundLoadQueue_.Find(*i);
						if (j != backgroundLoadQueue_.End())   //����������Դ�ں�̨���м�����
							j->second_.dependencies_.Erase(key);    //�ӵ����ߵ������б����Ƴ�����Դ
					}

					item.dependents_.Clear();
				}
				//���ü���״̬
				resource->SetAsyncLoadState(success ? ASYNC_SUCCESS : ASYNC_FAIL);
				backgroundLoadMutex_.Release();
			}
		}
	}

	//����Դ������ض���
	bool BackgroundLoader::QueueResource(StringHash type, const String& name, bool sendEventOnFailure, Resource* caller)
	{
		StringHash nameHash(name);
		Pair<StringHash, StringHash> key = MakePair(type, nameHash);

		MutexLock lock(backgroundLoadMutex_);

		// �Ƿ񲻴���
		if (backgroundLoadQueue_.Find(key) != backgroundLoadQueue_.End())
			return false;

		//ȡ���ú�̨������
		BackgroundLoadItem& item = backgroundLoadQueue_[key];
		//���÷���ʧ���¼�
		item.sendEventOnFailure_ = sendEventOnFailure;

		// ȷ��ָ��ǿղ�������Դ������
		item.resource_ = DynamicCast<Resource>(owner_->GetContext()->CreateObject(type)); //����һ������Դ����
		if (!item.resource_)
		{
			//����ʧ��
			SAPPHIRE_LOGERROR("Could not load unknown resource type " + String(type));

			if (sendEventOnFailure && Thread::IsMainThread())
			{
				using namespace UnknownResourceType;

				VariantMap& eventData = owner_->GetEventDataMap();
				eventData[P_RESOURCETYPE] = type;
				owner_->SendEvent(E_UNKNOWNRESOURCETYPE, eventData);
			}
			//�Ӷ������Ƴ�
			backgroundLoadQueue_.Erase(key);
			return false;
		}

		SAPPHIRE_LOGDEBUG("Background loading resource " + name);

		//������Դ��
		item.resource_->SetName(name);
		//����״̬
		item.resource_->SetAsyncLoadState(ASYNC_QUEUED);

		// ��������Դ�б�����ĺ�̨��Դ���ã� ������������ϵ
		if (caller)  //������
		{
			Pair<StringHash, StringHash> callerKey = MakePair(caller->GetType(), caller->GetNameHash());
			HashMap<Pair<StringHash, StringHash>, BackgroundLoadItem>::Iterator j = backgroundLoadQueue_.Find(callerKey);
			if (j != backgroundLoadQueue_.End())
			{
				BackgroundLoadItem& callerItem = j->second_;
				item.dependents_.Insert(callerKey);      //Ϊ����Դ��ӵ����߹�ϵ
				callerItem.dependencies_.Insert(key);  //Ϊ���������������Դ
			}
			else
				SAPPHIRE_LOGWARNING("Resource " + caller->GetName() +
				" requested for a background loaded resource but was not in the background load queue");
		}

		// ���̨�����߳�û��ʼ������
		if (!IsStarted())
			Run();

		return true;
	}

	//�ȴ���Դ�����
	void BackgroundLoader::WaitForResource(StringHash type, StringHash nameHash)
	{
		backgroundLoadMutex_.Acquire();

		// ��ѯ��Դ�Ƿ��ڼ���״̬
		Pair<StringHash, StringHash> key = MakePair(type, nameHash);
		HashMap<Pair<StringHash, StringHash>, BackgroundLoadItem>::Iterator i = backgroundLoadQueue_.Find(key);
		if (i != backgroundLoadQueue_.End())
		{
			backgroundLoadMutex_.Release();

			{
				Resource* resource = i->second_.resource_;
				HiresTimer waitTimer;   //�ȴ���ʱ��
				bool didWait = false;

				for (;;)
				{
					//������Դ������
					unsigned numDeps = i->second_.dependencies_.Size();
					AsyncLoadState state = resource->GetAsyncLoadState();  //��ȡ�첽����״̬
					if (numDeps > 0 || state == ASYNC_QUEUED || state == ASYNC_LOADING)
					{
						//δ������ɣ���΢�ȴ�һ��
						didWait = true;
						Time::Sleep(1);
					}
					else
						break;  //��ɼ��ػ�ʧ���˳�ѭ��
				}

				if (didWait) //���س�ʱ��
					SAPPHIRE_LOGDEBUG("Waited " + String(waitTimer.GetUSec(false) / 1000) + " ms for background loaded resource " +
					resource->GetName());
			}

			// ���ܻ��Ϻܳ�ʱ��ȴ�������Դ�����ܷ�mutex
			FinishBackgroundLoading(i->second_);

			backgroundLoadMutex_.Acquire();
			backgroundLoadQueue_.Erase(i);  //ɾ���ü�����Ŀ
			backgroundLoadMutex_.Release();
		}
		else
			backgroundLoadMutex_.Release();
	}

	//���������ɵ���Դ������
	void BackgroundLoader::FinishResources(int maxMs)
	{
		if (IsStarted())
		{
			HiresTimer timer;

			backgroundLoadMutex_.Acquire();

			for (HashMap<Pair<StringHash, StringHash>, BackgroundLoadItem>::Iterator i = backgroundLoadQueue_.Begin();
				i != backgroundLoadQueue_.End();)
			{
				Resource* resource = i->second_.resource_;
				unsigned numDeps = i->second_.dependencies_.Size();
				AsyncLoadState state = resource->GetAsyncLoadState();
				if (numDeps > 0 || state == ASYNC_QUEUED || state == ASYNC_LOADING)
					++i;
				else
				{
					// �ս�һ����Դ������Ҫ�ȴ���������Դ�ļ��أ� ���ﲻ�Ż�����
					backgroundLoadMutex_.Release();
					FinishBackgroundLoading(i->second_);
					backgroundLoadMutex_.Acquire();
					i = backgroundLoadQueue_.Erase(i);
				}

				// ��ʱ�ж�
				if (timer.GetUSec(false) >= maxMs * 1000)
					break;
			}

			backgroundLoadMutex_.Release();
		}
	}

	//��ȡ���ض��е���Դ����Ŀ
	unsigned BackgroundLoader::GetNumQueuedResources() const
	{
		MutexLock lock(backgroundLoadMutex_);
		return backgroundLoadQueue_.Size();
	}

	//�ս������
	void BackgroundLoader::FinishBackgroundLoading(BackgroundLoadItem& item)
	{
		Resource* resource = item.resource_;

		bool success = resource->GetAsyncLoadState() == ASYNC_SUCCESS;
		// ���BeginLoad() �׶γɹ�������EndLoad()�ս�ɹ���ʧ�ܵĽ��
		if (success)
		{
#ifdef SAPPHIRE_PROFILING
			String profileBlockName("Finish" + resource->GetTypeName());

			Profiler* profiler = owner_->GetSubsystem<Profiler>();
			if (profiler)
				profiler->BeginBlock(profileBlockName.CString());
#endif
			SAPPHIRE_LOGDEBUG("Finishing background loaded resource " + resource->GetName());
			success = resource->EndLoad();

#ifdef SAPPHIRE_PROFILING
			if (profiler)
				profiler->EndBlock();
#endif
		}
		//������Դ״̬
		resource->SetAsyncLoadState(ASYNC_DONE);
		//���ɹ������¼�
		if (!success && item.sendEventOnFailure_)
		{
			using namespace LoadFailed;

			VariantMap& eventData = owner_->GetEventDataMap();
			eventData[P_RESOURCENAME] = resource->GetName();
			owner_->SendEvent(E_LOADFAILED, eventData);
		}

		// �����¼������˳ɹ�����ʧ��
	{
		using namespace ResourceBackgroundLoaded;

		VariantMap& eventData = owner_->GetEventDataMap();
		eventData[P_RESOURCENAME] = resource->GetName();
		eventData[P_SUCCESS] = success;
		eventData[P_RESOURCE] = resource;
		owner_->SendEvent(E_RESOURCEBACKGROUNDLOADED, eventData);
	}

	//���浽��Դ����ϵͳ�У� ʹ���ֶ������Դ�ķ�ʽ
	if (success || owner_->GetReturnFailedResources())
		owner_->AddManualResource(resource);
	}

}

#endif