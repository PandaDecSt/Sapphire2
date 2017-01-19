#pragma once


#include "LinkedList.h"
#include "Variant.h"

namespace Sapphire
{
	class Context;
	class EventHandler;

	/// ��������ʱ������Ϣ
	class SAPPHIRE_API TypeInfo
	{
	public:
		
		TypeInfo(const char* typeName, const TypeInfo* baseTypeInfo);
	
		~TypeInfo();

		// ��鵱ǰ�����ǲ���ָ��������
		bool IsTypeOf(StringHash type) const;
		/// ��鵱ǰ�����ǲ���ָ������
		bool IsTypeOf(const TypeInfo* typeInfo) const;
		/// ��鵱ǰ�����ǲ���ָ����������
		template<typename T> bool IsTypeOf() const { return IsTypeOf(T::GetTypeInfoStatic()); }

		/// ��������hash
		StringHash GetType() const { return type_; }
		/// ����������
		const String& GetTypeName() const { return typeName_; }
		/// ���ػ�������Ϣ
		const TypeInfo* GetBaseTypeInfo() const { return baseTypeInfo_; }

	private:
		/// ����hashֵ
		StringHash type_;
		/// ������
		String typeName_;
		/// ����������Ϣ
		const TypeInfo* baseTypeInfo_;
	};

	//��ԭ�������ͻ�������������ΪClassName��BaseClassName����Object�ڲ�ͳһ����
#define SAPPHIRE_OBJECT(typeName, baseTypeName) \
    public: \
        typedef typeName ClassName; \
        typedef baseTypeName BaseClassName; \
        virtual Sapphire::StringHash GetType() const { return GetTypeInfoStatic()->GetType(); } \
        virtual const Sapphire::String& GetTypeName() const { return GetTypeInfoStatic()->GetTypeName(); } \
        virtual const Sapphire::TypeInfo* GetTypeInfo() const { return GetTypeInfoStatic(); } \
        static Sapphire::StringHash GetTypeStatic() { return GetTypeInfoStatic()->GetType(); } \
        static const Sapphire::String& GetTypeNameStatic() { return GetTypeInfoStatic()->GetTypeName(); } \
        static const Sapphire::TypeInfo* GetTypeInfoStatic() { static const Sapphire::TypeInfo typeInfoStatic(#typeName, BaseClassName::GetTypeInfoStatic()); return &typeInfoStatic; } \


	//  �������Ļ��࣬�ṩ���Ͷ��壬��ϵͳ���ʺ��¼�����/���չ���
	class SAPPHIRE_API Object : public RefCounted
	{
		friend class Context;   //��Ԫ�����ڶ�Context˽�г�Ա���з���

	public:
		/// �û������������
		Object(Context* context);
		/// ����ʱ���������sender��receiver�¼��ṹ
		virtual ~Object();

		 
		virtual StringHash GetType() const = 0;
		 
		virtual const String& GetTypeName() const = 0;
		 
		virtual const TypeInfo* GetTypeInfo() const = 0;
		
		/// �¼���Ӧ����
		virtual void OnEvent(Object* sender, StringHash eventType, VariantMap& eventData);

		
		//  ����������Ϣ��̬
		static const TypeInfo* GetTypeInfoStatic() { return 0; }
		/// ��鵱ǰ�����Ƿ���ָ������
		static bool IsTypeOf(StringHash type);
		///��鵱ǰ�����Ƿ���ָ������
		static bool IsTypeOf(const TypeInfo* typeInfo);
		///��鵱ǰ�����Ƿ���ָ����
		template<typename T> static bool IsTypeOf() { return IsTypeOf(T::GetTypeInfoStatic()); }
		/// ��鵱ǰʵ���Ƿ���ָ������
		bool IsInstanceOf(StringHash type) const;
		/// ��鵱ǰʵ���Ƿ���ָ������
		bool IsInstanceOf(const TypeInfo* typeInfo) const;
		/// ��鵱ǰʵ���Ƿ���ָ����
		template<typename T> bool IsInstanceOf() const { return IsInstanceOf(T::GetTypeInfoStatic()); }

		
		// ���Ŀ������κη����߷��͵�һ���¼�
		void SubscribeToEvent(StringHash eventType, EventHandler* handler);
		///  ���Ŀ������ض������߷��͵�һ���¼�
		void SubscribeToEvent(Object* sender, StringHash eventType, EventHandler* handler);
		/// ȡ�������¼�
		void UnsubscribeFromEvent(StringHash eventType);
		/// ȡ���ض������ߵ��ض��¼����͵��¼�
		void UnsubscribeFromEvent(Object* sender, StringHash eventType);
		/// ȡ���ض������ߵ����ж��ĵ��¼�
		void UnsubscribeFromEvents(Object* sender);
		/// ȡ�����ĵ������¼�
		void UnsubscribeFromAllEvents();
		/// ȡ�����ĳ����б��ڵ������¼�����ֻ���û����ݵ�
		void UnsubscribeFromAllEventsExcept(const PODVector<StringHash>& exceptions, bool onlyUserData);
		/// �����ж����߷����¼�
		void SendEvent(StringHash eventType);
		/// ��ָ���Ĳ��������еĶ����߷����¼�
		void SendEvent(StringHash eventType, VariantMap& eventData);
		/// ����һ��Ԥ������¼�����ӳ�䡣 
		VariantMap& GetEventDataMap() const;

		/// ����ִ�л���Context
		Context* GetContext() const { return context_; }

		 
		// ͨ�����ͷ�����ϵͳ
		Object* GetSubsystem(StringHash type) const;
		/// ���ػ���¼�������,δ�¼������ΪNULL
		Object* GetEventSender() const;
		/// ���ػ���¼�������,δ�¼������ΪNULL
		EventHandler* GetEventHandler() const;
		//  �����Ƿ�����һ��ָ�����͵��¼�
		bool HasSubscribedToEvent(StringHash eventType) const;
		/// �����Ƿ�����һ��ָ�������ߺ�ָ�����͵��¼�
		bool HasSubscribedToEvent(Object* sender, StringHash eventType) const;

		/// �����Ƿ�����һ��������¼�
		bool HasEventHandlers() const { return !eventHandlers_.Empty(); }

		// ����һ����ϵͳ��ģ��汾
		template <class T> T* GetSubsystem() const;
		// ���ض���Ŀ¼�� Ŀ¼ͨ�����󹤳�ע�ᡣ�������Ŀ¼û��ע�᷵�ؿ�
		const String& GetCategory() const;

	protected:
		/// ִ�л���
		Context* context_;

	private:
		// ����û���ض������ߵĵ�һ���¼�������
		EventHandler* FindEventHandler(StringHash eventType, EventHandler** previous = 0) const;
		///�������ض������ߵĵ�һ���¼�������
		EventHandler* FindSpecificEventHandler(Object* sender, EventHandler** previous = 0) const;
		/// �������ض������ߺ��ض����͵ĵ�һ���¼�������
		EventHandler* FindSpecificEventHandler(Object* sender, StringHash eventType, EventHandler** previous = 0) const;

		// �Ƴ�һ���ض���������ص��¼�������
		void RemoveEventSender(Object* sender);

		 
		//�¼��������б�,  ���ڷ��ض��Ĵ�����senderΪnull
		LinkedList<EventHandler> eventHandlers_;
	};

	template <class T> T* Object::GetSubsystem() const { return static_cast<T*>(GetSubsystem(T::GetTypeStatic())); }

	 
	// �����󹤳�
	class SAPPHIRE_API ObjectFactory : public RefCounted
	{
	public:
		/// Construct.
		ObjectFactory(Context* context) :
			context_(context)
		{
			assert(context_);
		}

		/// ����һ��������ģ��������ʵ��
		virtual SharedPtr<Object> CreateObject() = 0;

		/// ����ִ�л���
		Context* GetContext() const { return context_; }

		/// ͨ�����󹤳����ش����Ķ����������Ϣ
		const TypeInfo* GetTypeInfo() const { return typeInfo_; }

		/// ͨ�����󹤳����ش������������hash
		StringHash GetType() const { return typeInfo_->GetType(); }

		/// ͨ�����󹤳����ش��������������
		const String& GetTypeName() const { return typeInfo_->GetTypeName(); }

	protected:
		/// ִ�л���
		Context* context_;
		/// ������Ϣ
		const TypeInfo* typeInfo_;
	};

	// ���󹤳���ģ��ʵ��
	template <class T> class ObjectFactoryImpl : public ObjectFactory
	{
	public:
		 
		ObjectFactoryImpl(Context* context) :
			ObjectFactory(context)
		{
			typeInfo_ = T::GetTypeInfoStatic();  //��ȡ��Ӧ���͵���������Ϣ
		}

		// ����һ���ض����͵Ķ���
		virtual SharedPtr<Object> CreateObject() { return SharedPtr<Object>(new T(context_)); }
	};

	// ����ִ���¼��������������ڲ�������
	class SAPPHIRE_API EventHandler : public LinkedListNode
	{
	public:
		// ��ָ����receiver������
		EventHandler(Object* receiver) :
			receiver_(receiver),
			sender_(0),
			userData_(0)
		{
			assert(receiver_);
		}

		///  ��ָ����receiver��userData������
		EventHandler(Object* receiver, void* userData) :
			receiver_(receiver),
			sender_(0),
			userData_(userData)
		{
			assert(receiver_);
		}

		 
		virtual ~EventHandler() { }

		 
		void SetSenderAndEventType(Object* sender, StringHash eventType)
		{
			sender_ = sender;
			eventType_ = eventType;
		}

		///ִ���¼�������
		virtual void Invoke(VariantMap& eventData) = 0;

		// ����һ���¼���������Ψһ����
		virtual EventHandler* Clone() const = 0;

		/// ����receiver
		Object* GetReceiver() const { return receiver_; }

		 
		Object* GetSender() const { return sender_; }

	 
		const StringHash& GetEventType() const { return eventType_; }

	 
		void* GetUserData() const { return userData_; }

	protected:
		 
		Object* receiver_;
	 
		Object* sender_;
		 
		StringHash eventType_;
		 
		void* userData_;
	};

	
	// �¼�����������ִ�е�ģ��ʵ����(����һ��ָ����ĺ���ָ��)
	template <class T> class EventHandlerImpl : public EventHandler
	{
	public:
		typedef void (T::*HandlerFunctionPtr)(StringHash, VariantMap&);

		
		//��receiver�ͺ���ָ�빹��
		EventHandlerImpl(T* receiver, HandlerFunctionPtr function) :
			EventHandler(receiver),
			function_(function)
		{
			assert(function_);
		}

			// ����һ���¼���������Ψһ����
		EventHandlerImpl(T* receiver, HandlerFunctionPtr function, void* userData) :
			EventHandler(receiver, userData),
			function_(function)
		{
			assert(function_);
		}

		
		// ͨ������ָ��ִ���¼�������
		virtual void Invoke(VariantMap& eventData)
		{
			T* receiver = static_cast<T*>(receiver_);
			(receiver->*function_)(eventType_, eventData);
		}

		// ����һ���¼���������Ψһ����
		virtual EventHandler* Clone() const
		{
			return new EventHandlerImpl(static_cast<T*>(receiver_), function_, userData_);
		}

	private:
		// ָ�������ĺ���ָ��
		HandlerFunctionPtr function_;
	};

	/// ����һ���¼���hash ID  �� ��ʼһ���������Ĳ����������ռ�
#define SAPPHIRE_EVENT(eventID, eventName) static const Sapphire::StringHash eventID(#eventName); namespace eventName
	/// ����һ������Ĳ���hash ID��Ӧ�ñ�����һ���¼��������ռ��
#define SAPPHIRE_PARAM(paramID, paramName) static const Sapphire::StringHash paramID(#paramName)
	/// ���ڹ���ָ��һ��receiver��������ĳ�Ա�������¼��������ĺ�
#define SAPPHIRE_HANDLER(className, function) (new Sapphire::EventHandlerImpl<className>(this, &className::function))
	/// ���ڹ���ָ��һ��receiver��������ĳ�Ա�������¼��������ĺ꣬���Ҷ������û�����
#define SAPPHIRE_HANDLER_USERDATA(className, function, userData) (new Sapphire::EventHandlerImpl<className>(this, &className::function, userData))

}