#pragma once

#include "Sapphire.h"

#include "Allocator.h"
#include "Hash.h"
#include "Swap.h"


namespace Sapphire
{
	/// Hash set/map node base class.
	//  hash set/map �ڵ����
	struct HashNodeBase
	{
		/// Construct.
		HashNodeBase() :
			down_(0),
			prev_(0),
			next_(0)
		{
		}

		/// Next node in the bucket.
		//���Ͱ����¸��ڵ� 
		HashNodeBase* down_;
		/// Previous node.
		//  ǰһ���ڵ�
		HashNodeBase* prev_;
		/// Next node.
		//  �¸��ڵ�
		HashNodeBase* next_;
	};

	/// Hash set/map iterator base class.
	//  hash set/map ����������
	struct HashIteratorBase
	{
		/// Construct.
		HashIteratorBase() :
			ptr_(0)
		{
		}

		/// Construct with a node pointer.
		//  ��һ���ڵ�ָ��Ĺ�����
		explicit HashIteratorBase(HashNodeBase* ptr) :
			ptr_(ptr)
		{
		}

		/// Test for equality with another iterator.
		//  �Ƚ�ָ��ֵ
		bool operator ==(const HashIteratorBase& rhs) const { return ptr_ == rhs.ptr_; }

		/// Test for inequality with another iterator.
		// �Ƚ�ָ��ֵ
		bool operator !=(const HashIteratorBase& rhs) const { return ptr_ != rhs.ptr_; }

		/// Go to the next node.
		//  ����һ�ڵ�
		void GotoNext()
		{
			if (ptr_)
				ptr_ = ptr_->next_;
		}

		/// Go to the previous node.
		//  ����һ�ڵ�
		void GotoPrev()
		{
			if (ptr_)
				ptr_ = ptr_->prev_;
		}

		/// Node pointer.
		//  �ڵ�ָ��
		HashNodeBase* ptr_;
	};

	/// Hash set/map base class.
	//  hash set/map ����
	/** Note that to prevent extra memory use due to vtable pointer, %HashBase intentionally does not declare a virtual destructor
	and therefore %HashBase pointers should never be used.
	    ע��
	*/
	class SAPPHIRE_API HashBase
	{
	public:
		/// Initial amount of buckets.
		//  ��ʼ��һ��������Ͱ
		static const unsigned MIN_BUCKETS = 8;
		/// Maximum load factor.
		//  ����������
		static const unsigned MAX_LOAD_FACTOR = 4;

		/// Construct.
		HashBase() :
			ptrs_(0),
			allocator_(0)
		{
		}

		/// Swap with another hash set or map.
		//�������Ľ��н���
		void Swap(HashBase& rhs)
		{
			Sapphire::Swap(head_, rhs.head_);
			Sapphire::Swap(tail_, rhs.tail_);
			Sapphire::Swap(ptrs_, rhs.ptrs_);
			Sapphire::Swap(allocator_, rhs.allocator_);
		}

		/// Return number of elements.
		// ����Ԫ������
		unsigned Size() const { return ptrs_ ? (reinterpret_cast<unsigned*>(ptrs_))[0] : 0; }

		/// Return number of buckets.
		//  ����Ͱ����
		unsigned NumBuckets() const { return ptrs_ ? (reinterpret_cast<unsigned*>(ptrs_))[1] : 0; }

		/// Return whether has no elements.
		bool Empty() const { return Size() == 0; }

	protected:
		/// Allocate bucket head pointers + room for size and bucket count variables.
		void AllocateBuckets(unsigned size, unsigned numBuckets);

		/// Reset bucket head pointers.
		void ResetPtrs();

		/// Set new size.
		void SetSize(unsigned size) { if (ptrs_) (reinterpret_cast<unsigned*>(ptrs_))[0] = size; }

		/// Return bucket head pointers.
		HashNodeBase** Ptrs() const { return ptrs_ ? ptrs_ + 2 : 0; }

		/// List head node pointer.
		//  ͷ�ڵ�ָ��
		HashNodeBase* head_;
		/// List tail node pointer.
		//  β���ڵ�ָ��
		HashNodeBase* tail_;
		/// Bucket head pointers.
		//  Ͱָ��
		HashNodeBase** ptrs_;
		/// Node allocator.
		//  �ڵ������
		AllocatorBlock* allocator_;
	};
}