#pragma once

#include "Attribute.h"
#include "Object.h"
#include "HashSet.h"


namespace Sapphire
{

	
	//ִ�������� ���ṩ��ϵͳ���ʣ����󹤳������ԣ��¼�������
	class SAPPHIRE_API Context : public RefCounted
	{
		friend class Object;

	public:
		 
		Context();
		 
		~Context();

		
		//ͨ������hash������һ���������û�ҵ���������null
		SharedPtr<Object> CreateObject(StringHash objectType);
		//Ϊһ����������ע��һ������
		void RegisterFactory(ObjectFactory* factory);
		// ע��һ�����ص�Ŀ¼�Ķ��󹤳�
		void RegisterFactory(ObjectFactory* factory, const char* category);
		/// ע��һ����ϵͳ
		void RegisterSubsystem(Object* subsystem);
		/// �Ƴ���ϵͳ
		void RemoveSubsystem(StringHash objectType);
		/// ע���������
		// ��֧�ֿջ�ָ������
		void RegisterAttribute(StringHash objectType, const AttributeInfo& attr);
		/// �Ƴ����������
		void RemoveAttribute(StringHash objectType, const char* name);
		/// ��Ĭ��ֵ���¶�������
		void UpdateAttributeDefaultValue(StringHash objectType, const char* name, const Variant& defaultValue);
		/// ����Ԥ������¼�����ӳ�䡣�����Ż��¼�����ӳ�䣬���⾭���ط���
		VariantMap& GetEventDataMap();

		
		// ���ƻ������Ե�������
		void CopyBaseAttributes(StringHash baseType, StringHash derivedType);
		// ע��һ�����󹤳���ģ��汾
		template <class T> void RegisterFactory();
		///ע��һ�����ص�Ŀ¼�Ķ��󹤳���ģ��汾
		template <class T> void RegisterFactory(const char* category);
		/// �Ƴ���ϵͳ��ģ��汾 
		template <class T> void RemoveSubsystem();
	    // ע���������Tע������
		template <class T> void RegisterAttribute(const AttributeInfo& attr);
	    // �Ƴ���������T������
		template <class T> void RemoveAttribute(const char* name);
		//���ƻ�����
		template <class T, class U> void CopyBaseAttributes();
		//����Ĭ������
		template <class T> void UpdateAttributeDefaultValue(const char* name, const Variant& defaultValue);

		
		// ������ϵͳ����
		Object* GetSubsystem(StringHash type) const;

		/// �������е���ϵͳ
		const HashMap<StringHash, SharedPtr<Object> >& GetSubsystems() const { return subsystems_; }

		/// �������еĶ��󹤳�
		const HashMap<StringHash, SharedPtr<ObjectFactory> >& GetObjectFactories() const { return factories_; }

		/// �������еĶ���Ŀ¼
		const HashMap<String, Vector<StringHash> >& GetObjectCategories() const { return objectCategories_; }

		/// ���ػ���¼������ߣ����¼�������ΪNULL
		Object* GetEventSender() const;

		/// ���ػ���¼����������ɶ������á� ���¼�������ΪNULL
		EventHandler* GetEventHandler() const { return eventHandler_; }


		// ��hashֵ�����ض��������������unknownΪ��
		const String& GetTypeName(StringHash objectType) const;
		//  ����һ�������һ���ض�������,���û�ҵ�ΪNULL
		AttributeInfo* GetAttribute(StringHash objectType, const char* name);
		/// Template version of returning a subsystem.
		// ȡ����ϵͳ��ģ��汾
		template <class T> T* GetSubsystem() const;		 
		// ��ȡһ��ָ�����Ե�ģ��汾
		template <class T> AttributeInfo* GetAttribute(const char* name);

		// ����һ���������͵��������������û�ж��巵�ؿ�
		const Vector<AttributeInfo>* GetAttributes(StringHash type) const
		{
			HashMap<StringHash, Vector<AttributeInfo> >::ConstIterator i = attributes_.Find(type);
			return i != attributes_.End() ? &i->second_ : 0;
		}

		// ����һ���������͵����縴�Ƶ����ԣ����û�ж��巵�ؿ�
		const Vector<AttributeInfo>* GetNetworkAttributes(StringHash type) const
		{
			HashMap<StringHash, Vector<AttributeInfo> >::ConstIterator i = networkAttributes_.Find(type);
			return i != networkAttributes_.End() ? &i->second_ : 0;
		}

		//  �������е�ע�������
		const HashMap<StringHash, Vector<AttributeInfo> >& GetAllAttributes() const { return attributes_; }

		// ����һ�������ߺ��¼����͵Ķ�Ӧ�¼������ߣ���������ڷ��ؿ�
		HashSet<Object*>* GetEventReceivers(Object* sender, StringHash eventType)
		{
			HashMap<Object*, HashMap<StringHash, HashSet<Object*> > >::Iterator i = specificEventReceivers_.Find(sender);
			if (i != specificEventReceivers_.End())
			{
				HashMap<StringHash, HashSet<Object*> >::Iterator j = i->second_.Find(eventType);
				return j != i->second_.End() ? &j->second_ : 0;
			}
			else
				return 0;
		}

		// ����һ���¼����͵�receiver����������ڷ���null
		HashSet<Object*>* GetEventReceivers(StringHash eventType)
		{
			HashMap<StringHash, HashSet<Object*> >::Iterator i = eventReceivers_.Find(eventType);
			return i != eventReceivers_.End() ? &i->second_ : 0;
		}

	private:
	 
		// ���һ���¼�������
		void AddEventReceiver(Object* receiver, StringHash eventType);
		/// ���һ��ָ���¼����͵�Receiver
		void AddEventReceiver(Object* receiver, Object* sender, StringHash eventType);
		// �Ƴ�һ���¼������ߵ�����receiver�� ������������
		void RemoveEventSender(Object* sender);
		// �Ƴ�һ��ָ���¼����¼�������
		void RemoveEventReceiver(Object* receiver, Object* sender, StringHash eventType);
		// �ӷ��ض��¼��Ƴ��¼�������
		void RemoveEventReceiver(Object* receiver, StringHash eventType);

		// ���õ�ǰ�¼��������� ͨ���������
		void SetEventHandler(EventHandler* handler) { eventHandler_ = handler; }

		/// ��ʼ�����¼�
		void BeginSendEvent(Object* sender) { eventSenders_.Push(sender); }

		/// �����¼��ķ��͡��ڴ�ͬʱ�����¼�������
		void EndSendEvent() { eventSenders_.Pop(); }

		/// ���󹤳�
		HashMap<StringHash, SharedPtr<ObjectFactory> > factories_;
		/// ��ϵͳ.
		HashMap<StringHash, SharedPtr<Object> > subsystems_;
		/// ÿ���������͵İ󶨵���������
		HashMap<StringHash, Vector<AttributeInfo> > attributes_;
		/// ÿ���������͵����縴����������
		HashMap<StringHash, Vector<AttributeInfo> > networkAttributes_;
		/// ���ض��¼����¼�������
		HashMap<StringHash, HashSet<Object*> > eventReceivers_;
		/// ����ָ���˷����ߵ��¼�������
		HashMap<Object*, HashMap<StringHash, HashSet<Object*> > > specificEventReceivers_;
		/// �¼�������ջ
		PODVector<Object*> eventSenders_;
		/// �¼�����ջ
		PODVector<VariantMap*> eventDataMaps_;
		/// ����¼�����������浽ջ����Ҫ������ԭ��
		EventHandler* eventHandler_;
		///����Ŀ¼
		HashMap<String, Vector<StringHash> > objectCategories_;
	};

	//���Զ�����һ������T�Ķ��󹤳�������ӵ����󹤳��б���
	template <class T> void Context::RegisterFactory() { RegisterFactory(new ObjectFactoryImpl<T>(this)); }
	//ָ��Ŀ¼ע����󹤳�
	template <class T> void Context::RegisterFactory(const char* category)
	{
		RegisterFactory(new ObjectFactoryImpl<T>(this), category);
	}

	template <class T> void Context::RemoveSubsystem() { RemoveSubsystem(T::GetTypeStatic()); }

	template <class T> void Context::RegisterAttribute(const AttributeInfo& attr) { RegisterAttribute(T::GetTypeStatic(), attr); }

	template <class T> void Context::RemoveAttribute(const char* name) { RemoveAttribute(T::GetTypeStatic(), name); }
	//����������T�����Ը��Ƹ�����U
	template <class T, class U> void Context::CopyBaseAttributes() { CopyBaseAttributes(T::GetTypeStatic(), U::GetTypeStatic()); }

	template <class T> T* Context::GetSubsystem() const { return static_cast<T*>(GetSubsystem(T::GetTypeStatic())); }
	//��ȡ������
	template <class T> AttributeInfo* Context::GetAttribute(const char* name) { return GetAttribute(T::GetTypeStatic(), name); }

	template <class T> void Context::UpdateAttributeDefaultValue(const char* name, const Variant& defaultValue)
	{
		UpdateAttributeDefaultValue(T::GetTypeStatic(), name, defaultValue);
	}

}
