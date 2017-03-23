
#include "Predefined.h"

#include "Context.h"
#include "Thread.h"
#include "Log.h"

#include "DebugNew.h"

namespace Sapphire
{

	TypeInfo::TypeInfo(const char* typeName, const TypeInfo* baseTypeInfo) :
		type_(typeName),
		typeName_(typeName),
		baseTypeInfo_(baseTypeInfo)
	{
	}

	TypeInfo::~TypeInfo()
	{
	}

	bool TypeInfo::IsTypeOf(StringHash type) const
	{
		const TypeInfo* current = this;
		while (current)
		{
			if (current->GetType() == type)
				return true;

			current = current->GetBaseTypeInfo();
		}

		return false;
	}

	bool TypeInfo::IsTypeOf(const TypeInfo* typeInfo) const
	{
		const TypeInfo* current = this;
		while (current)
		{
			if (current == typeInfo)
				return true;

			current = current->GetBaseTypeInfo();
		}

		return false;
	}

	Object::Object(Context* context) :
		context_(context)
	{
		assert(context_);
	}

	Object::~Object()
	{
		UnsubscribeFromAllEvents();
		context_->RemoveEventSender(this);
	}

	void Object::OnEvent(Object* sender, StringHash eventType, VariantMap& eventData)
	{
		 
		//�Ի���ָ����һ���������Է����¼����ִ�е�ʱ�����������
		Context* context = context_;
		//���ض��¼�������
		EventHandler* specific = 0;
		EventHandler* nonSpecific = 0;

		EventHandler* handler = eventHandlers_.First();
		while (handler)
		{
			if (handler->GetEventType() == eventType)
			{
				if (!handler->GetSender())
					nonSpecific = handler;
				else if (handler->GetSender() == sender)
				{
					specific = handler;
					break;
				}
			}
			handler = eventHandlers_.Next(handler);
		}

		
		// �ض����¼���������������Ȩ�����û�ҵ���ִ�е�һ��
		if (specific)
		{
			context->SetEventHandler(specific);
			specific->Invoke(eventData);  //ִ���¼�������
			context->SetEventHandler(0);
			return;
		}

		if (nonSpecific)
		{
			context->SetEventHandler(nonSpecific);
			nonSpecific->Invoke(eventData);
			context->SetEventHandler(0);
		}
	}

	bool Object::IsTypeOf(StringHash type)
	{
		return GetTypeInfoStatic()->IsTypeOf(type);
	}

	bool Object::IsTypeOf(const TypeInfo* typeInfo)
	{
		return GetTypeInfoStatic()->IsTypeOf(typeInfo);
	}

	bool Object::IsInstanceOf(StringHash type) const
	{
		return GetTypeInfo()->IsTypeOf(type);
	}

	bool Object::IsInstanceOf(const TypeInfo* typeInfo) const
	{
		return GetTypeInfo()->IsTypeOf(typeInfo);
	}

	void Object::SubscribeToEvent(StringHash eventType, EventHandler* handler)
	{
		if (!handler)
			return;

		handler->SetSenderAndEventType(0, eventType);
		// �����Ƴ��ɵ��¼�������
		EventHandler* previous;
		// ���Ҹö����Ƿ��Ѿ�ע����¼����͵�Handler
		EventHandler* oldHandler = FindSpecificEventHandler(0, eventType, &previous); //��ѯ�͵��¼�������
		if (oldHandler)
			eventHandlers_.Erase(oldHandler, previous);

		eventHandlers_.InsertFront(handler);
		//���ø������¼��Ľ�����Ϊthis
		context_->AddEventReceiver(this, eventType);
	}

	void Object::SubscribeToEvent(Object* sender, StringHash eventType, EventHandler* handler)
	{
		// ���senderû��ָ��������¼����ܶ��ġ�ɾ�����������
		if (!sender || !handler)
		{
			delete handler;
			return;
		}

		handler->SetSenderAndEventType(sender, eventType);
		// �����Ƴ��ɵ�
		EventHandler* previous;
		EventHandler* oldHandler = FindSpecificEventHandler(sender, eventType, &previous);
		if (oldHandler)
			eventHandlers_.Erase(oldHandler, previous);

		eventHandlers_.InsertFront(handler);

		context_->AddEventReceiver(this, sender, eventType);
	}

	void Object::UnsubscribeFromEvent(StringHash eventType)
	{
		for (;;)
		{
			EventHandler* previous;
			//�ҵ������͵��¼�������
			EventHandler* handler = FindEventHandler(eventType, &previous);
			if (handler)
			{
				//Sender�Ƿ����
				if (handler->GetSender())
					context_->RemoveEventReceiver(this, handler->GetSender(), eventType);
				else
					context_->RemoveEventReceiver(this, eventType);
				eventHandlers_.Erase(handler, previous);
			}
			else
				break;
		}
	}

	void Object::UnsubscribeFromEvent(Object* sender, StringHash eventType)
	{
		if (!sender)
			return;

		EventHandler* previous;
		EventHandler* handler = FindSpecificEventHandler(sender, eventType, &previous);
		if (handler)
		{
			context_->RemoveEventReceiver(this, handler->GetSender(), eventType);
			eventHandlers_.Erase(handler, previous);
		}
	}

	void Object::UnsubscribeFromEvents(Object* sender)
	{
		if (!sender)
			return;

		for (;;)
		{
			EventHandler* previous;
			EventHandler* handler = FindSpecificEventHandler(sender, &previous);
			if (handler)
			{
				context_->RemoveEventReceiver(this, handler->GetSender(), handler->GetEventType());
				eventHandlers_.Erase(handler, previous);
			}
			else
				break;
		}
	}

	void Object::UnsubscribeFromAllEvents()
	{
		for (;;)
		{
			EventHandler* handler = eventHandlers_.First();
			if (handler)
			{
				if (handler->GetSender())
					context_->RemoveEventReceiver(this, handler->GetSender(), handler->GetEventType());
				else
					context_->RemoveEventReceiver(this, handler->GetEventType());
				eventHandlers_.Erase(handler);
			}
			else
				break;
		}
	}

	void Object::UnsubscribeFromAllEventsExcept(const PODVector<StringHash>& exceptions, bool onlyUserData)
	{
		EventHandler* handler = eventHandlers_.First();
		EventHandler* previous = 0;

		while (handler)
		{
			EventHandler* next = eventHandlers_.Next(handler);

			if ((!onlyUserData || handler->GetUserData()) && !exceptions.Contains(handler->GetEventType()))
			{
				if (handler->GetSender())
					context_->RemoveEventReceiver(this, handler->GetSender(), handler->GetEventType());
				else
					context_->RemoveEventReceiver(this, handler->GetEventType());

				eventHandlers_.Erase(handler, previous);
			}
			else
				previous = handler;

			handler = next;
		}
	}

	//�������¼����ݵ��¼�
	void Object::SendEvent(StringHash eventType)
	{
		VariantMap noEventData;

		SendEvent(eventType, noEventData);
	}

	//�������¼����ݵ��¼�
	void Object::SendEvent(StringHash eventType, VariantMap& eventData)
	{
		if (!Thread::IsMainThread())
		{
			SAPPHIRE_LOGERROR("Sending events is only supported from the main thread");
			return;
		}

		
		// Ϊ���¼���������٣�����һ��ָ���Լ�����ָ��
		WeakPtr<Object> self(this);
		Context* context = context_;
		HashSet<Object*> processed;

		context->BeginSendEvent(this);

		
		// ����һ���ض����¼�������
		const HashSet<Object*>* group = context->GetEventReceivers(this, eventType);
		if (group)
		{
			for (HashSet<Object*>::ConstIterator i = group->Begin(); i != group->End();)
			{
				HashSet<Object*>::ConstIterator current = i++;
				Object* receiver = *current;
				Object* next = 0;
				if (i != group->End())
					next = *i;

				unsigned oldSize = group->Size();
				receiver->OnEvent(this, eventType, eventData);

				// ��������Լ���Ϊһ���¼�����Ľṹ������,�˳�
				if (self.Expired())
				{
					context->EndSendEvent();
					return;
				}

				// If group has changed size during iteration (removed/added subscribers) try to recover
				/// \todo This is not entirely foolproof, as a subscriber could have been added to make up for the removed one
				if (group->Size() != oldSize)
					i = group->Find(next);

				processed.Insert(receiver);
			}
		}

		// ���ض��¼�������
		group = context->GetEventReceivers(eventType);
		if (group)
		{
			if (processed.Empty())
			{
				for (HashSet<Object*>::ConstIterator i = group->Begin(); i != group->End();)
				{
					HashSet<Object*>::ConstIterator current = i++;
					Object* receiver = *current;
					Object* next = 0;
					if (i != group->End())
						next = *i;

					unsigned oldSize = group->Size();
					receiver->OnEvent(this, eventType, eventData);

					if (self.Expired())
					{
						context->EndSendEvent();
						return;
					}

					if (group->Size() != oldSize)
						i = group->Find(next);
				}
			}
			else
			{
				// If there were specific receivers, check that the event is not sent doubly to them
				for (HashSet<Object*>::ConstIterator i = group->Begin(); i != group->End();)
				{
					HashSet<Object*>::ConstIterator current = i++;
					Object* receiver = *current;
					Object* next = 0;
					if (i != group->End())
						next = *i;

					if (!processed.Contains(receiver))
					{
						unsigned oldSize = group->Size();
						receiver->OnEvent(this, eventType, eventData);

						if (self.Expired())
						{
							context->EndSendEvent();
							return;
						}

						if (group->Size() != oldSize)
							i = group->Find(next);
					}
				}
			}
		}

		context->EndSendEvent();
	}

	VariantMap& Object::GetEventDataMap() const
	{
		return context_->GetEventDataMap();
	}

	Object* Object::GetSubsystem(StringHash type) const
	{
		return context_->GetSubsystem(type);
	}

	Object* Object::GetEventSender() const
	{
		return context_->GetEventSender();
	}

	EventHandler* Object::GetEventHandler() const
	{
		return context_->GetEventHandler();
	}

	bool Object::HasSubscribedToEvent(StringHash eventType) const
	{
		return FindEventHandler(eventType) != 0;
	}

	bool Object::HasSubscribedToEvent(Object* sender, StringHash eventType) const
	{
		if (!sender)
			return false;
		else
			return FindSpecificEventHandler(sender, eventType) != 0;
	}

	const String& Object::GetCategory() const
	{
		const HashMap<String, Vector<StringHash> >& objectCategories = context_->GetObjectCategories();
		for (HashMap<String, Vector<StringHash> >::ConstIterator i = objectCategories.Begin(); i != objectCategories.End(); ++i)
		{
			if (i->second_.Contains(GetType()))
				return i->first_;
		}

		return String::EMPTY;
	}

	EventHandler* Object::FindEventHandler(StringHash eventType, EventHandler** previous) const
	{
		EventHandler* handler = eventHandlers_.First();
		if (previous)
			*previous = 0;

		while (handler)
		{
			if (handler->GetEventType() == eventType)
				return handler;
			if (previous)
				*previous = handler;
			handler = eventHandlers_.Next(handler);
		}

		return 0;
	}

	EventHandler* Object::FindSpecificEventHandler(Object* sender, EventHandler** previous) const
	{
		EventHandler* handler = eventHandlers_.First();
		if (previous)
			*previous = 0;

		while (handler)
		{
			if (handler->GetSender() == sender)
				return handler;
			if (previous)
				*previous = handler;
			handler = eventHandlers_.Next(handler);
		}

		return 0;
	}

	EventHandler* Object::FindSpecificEventHandler(Object* sender, StringHash eventType, EventHandler** previous) const
	{
		EventHandler* handler = eventHandlers_.First();
		if (previous)
			*previous = 0;

		while (handler)
		{
			if (handler->GetSender() == sender && handler->GetEventType() == eventType)
				return handler;
			if (previous)
				*previous = handler;
			handler = eventHandlers_.Next(handler);
		}

		return 0;
	}

	void Object::RemoveEventSender(Object* sender)
	{
		EventHandler* handler = eventHandlers_.First();
		EventHandler* previous = 0;

		while (handler)
		{
			if (handler->GetSender() == sender)
			{
				EventHandler* next = eventHandlers_.Next(handler);
				eventHandlers_.Erase(handler, previous);
				handler = next;
			}
			else
			{
				previous = handler;
				handler = eventHandlers_.Next(handler);
			}
		}
	}

}