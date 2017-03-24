#pragma once

#include "../Core/Object.h"
#include "../Core/Timer.h"


namespace Sapphire
{
	class Deserializer;
	class Serializer;

	/// �첽����һ����Դ״̬
	enum AsyncLoadState
	{
		/// û���첽����
		ASYNC_DONE = 0,
		/// �첽���صĲ�ѯ
		ASYNC_QUEUED = 1,
		/// �ڹ����߳��е���BeginLoad()�Ľ���
		ASYNC_LOADING = 2,
		/// BeginLoad() �ɹ��� EndLoad()���ᱻ���̵߳���
		ASYNC_SUCCESS = 3,
		/// BeginLoad() ʧ��.
		ASYNC_FAIL = 4
	};

	///��Դ��Ļ���
	class SAPPHIRE_API Resource : public Object
	{
		SAPPHIRE_OBJECT(Resource, Object);

	public:
		 
		Resource(Context* context);

		/// ͬ��������Դ�� ����������ɹ�������BeginLoad() ��  EndLoad()������true
		bool Load(Deserializer& source);
		/// �����м�����Դ�� ���Դӹ����̱߳����á� �������true���
		virtual bool BeginLoad(Deserializer& source);
		/// ��Դ������ɡ�ͨ�������߳�����á� ���Ϊtrue����ɹ�
		virtual bool EndLoad();
		/// ������Դ������ɹ�����true
		virtual bool Save(Serializer& dest) const;

		/// ��������
		void SetName(const String& name);
		/// �����ڴ�ʹ���������Դ��ֵ
		void SetMemoryUse(unsigned size);
		/// �������ʹ�õ�timer
		void ResetUseTimer();
		/// �����첽����״̬�� ����Դ������á���Դ���첽���ص��в����������ظ��û�
		void SetAsyncLoadState(AsyncLoadState newState);

		/// ��������
		const String& GetName() const { return name_; }

		/// �������ֵ�Hash
		StringHash GetNameHash() const { return nameHash_; }

		/// �����ڴ�ʹ�����������ǽ���ֵ
		unsigned GetMemoryUse() const { return memoryUse_; }

		/// �����ϴ�ʹ�õ�ʱ��������� ������ò�����Դ���壬���Ƿ���0
		unsigned GetUseTimer();

		/// �����첽���ص�״̬
		AsyncLoadState GetAsyncLoadState() const { return asyncLoadState_; }

	private:
		//����
		String name_;
		/// ����hashֵ
		StringHash nameHash_;
		/// �ϴ�ʹ�õ�ʱ��
		Timer useTimer_;
		/// �ڴ�ʹ���ֽ�
		unsigned memoryUse_;
		/// �첽����״̬
		AsyncLoadState asyncLoadState_;
	};

	inline const String& GetResourceName(Resource* resource)
	{
		return resource ? resource->GetName() : String::EMPTY;
	}

	inline StringHash GetResourceType(Resource* resource, StringHash defaultType)
	{
		return resource ? resource->GetType() : defaultType;
	}

	inline ResourceRef GetResourceRef(Resource* resource, StringHash defaultType)
	{
		return ResourceRef(GetResourceType(resource, defaultType), GetResourceName(resource));
	}

	template <class T> Vector<String> GetResourceNames(const Vector<SharedPtr<T> >& resources)
	{
		Vector<String> ret(resources.Size());
		for (unsigned i = 0; i < resources.Size(); ++i)
			ret[i] = GetResourceName(resources[i]);

		return ret;
	}

	template <class T> ResourceRefList GetResourceRefList(const Vector<SharedPtr<T> >& resources)
	{
		return ResourceRefList(T::GetTypeStatic(), GetResourceNames(resources));
	}
}



