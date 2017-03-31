#include "../../Sapphire/Predefined.h"

#include "../Core/Profiler.h"
#include "../IO/Log.h"
#include "../Resource/Resource.h"


namespace Sapphire
{

	Resource::Resource(Context* context) :
		Object(context),
		memoryUse_(0),
		asyncLoadState_(ASYNC_DONE)
	{
	}

	bool Resource::Load(Deserializer& source)
	{
		// ����BeginLoad() / EndLoad()���ڹ����߳��е��ã�profiling����ղ���
		// ����������������profile��
#ifdef SAPPHIRE_PROFILING
		String profileBlockName("Load" + GetTypeName());

		Profiler* profiler = GetSubsystem<Profiler>();
		if (profiler)
			profiler->BeginBlock(profileBlockName.CString());
#endif
		//�Ƿ������߳̾����Ƿ�Ϊͬ�����첽
		SetAsyncLoadState(Thread::IsMainThread() ? ASYNC_DONE : ASYNC_LOADING);
		bool success = BeginLoad(source);  //�����м�����Դ
		if (success)
			success &= EndLoad(); 
		SetAsyncLoadState(ASYNC_DONE);

#ifdef SAPPHIRE_PROFILING
		if (profiler)
			profiler->EndBlock();
#endif

		return success;
	}

	bool Resource::BeginLoad(Deserializer& source)
	{
		// �����Ҫ�����าд
		return false;
	}

	bool Resource::EndLoad()
	{
		// ��������ϴ�GPU������Բ��ø�д
		return true;
	}

	bool Resource::Save(Serializer& dest) const
	{
		SAPPHIRE_LOGERROR("Save not supported for " + GetTypeName());
		return false;
	}

	void Resource::SetName(const String& name)
	{
		name_ = name;
		nameHash_ = name;
	}

	void Resource::SetMemoryUse(unsigned size)
	{
		memoryUse_ = size;
	}

	void Resource::ResetUseTimer()
	{
		useTimer_.Reset();
	}

	void Resource::SetAsyncLoadState(AsyncLoadState newState)
	{
		asyncLoadState_ = newState;
	}

	unsigned Resource::GetUseTimer()
	{
		// �������1��resource cache���ã� ����0����Timer
		if (Refs() > 1)
		{
			useTimer_.Reset();
			return 0;
		}
		else
			return useTimer_.GetMSec(false);
	}
}