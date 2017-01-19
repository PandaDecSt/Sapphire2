#pragma once

#include <Sapphire.h>

namespace Sapphire
{
	//Ӧ�ü���
	struct RefCount
	{
		 
		RefCount() :
			refs_(0),
			weakRefs_(0)
		{
		}

		~RefCount()
		{
			//������������Ȼ�ܷ��ʣ���������С��0
			refs_ = -1;
			weakRefs_ = -1;
		}
		//����
		int refs_;
		//������
		int weakRefs_;

	};

	class SAPPHIRE_API RefCounted
	{
	public:
		//����һ�����ü����ṹ����ʼ����һ������������
		RefCounted();
		//���٣����û���ⲿ�����ô��ڣ�ʹ��ʧЧ��ɾ�����ü����ṹ
		virtual ~RefCounted();

		/// Increment reference count. Can also be called outside of a SharedPtr for traditional reference counting.
		//  �������ü���.
		void AddRef();
		/// Decrement reference count and delete self if no more references. Can also be called outside of a SharedPtr for traditional reference counting.
		void ReleaseRef();
		/// Return reference count.
		int Refs() const;
		/// Return weak reference count.
		int WeakRefs() const;

		/// Return pointer to the reference count structure.
		RefCount* RefCountPtr() { return refCount_; }

	private:
		/// Prevent copy construction.
		RefCounted(const RefCounted& rhs);
		/// Prevent assignment.
		RefCounted& operator =(const RefCounted& rhs);

		/// Pointer to the reference count structure.
		RefCount* refCount_;
	};
}
