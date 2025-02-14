#pragma once


#include "Allocator.h"
#include "Swap.h"


namespace Sapphire
{
	/// Doubly-linked list node base class.
	struct ListNodeBase
	{
		/// Construct.
		ListNodeBase() :
			prev_(0),
			next_(0)
		{
		}

		/// Previous node.
		ListNodeBase* prev_;
		/// Next node.
		ListNodeBase* next_;
	};

	/// Doubly-linked list iterator base class.
	struct ListIteratorBase
	{
		/// Construct.
		ListIteratorBase() :
			ptr_(0)
		{
		}

		/// Construct with a node pointer.
		explicit ListIteratorBase(ListNodeBase* ptr) :
			ptr_(ptr)
		{
		}

		/// Test for equality with another iterator.
		bool operator ==(const ListIteratorBase& rhs) const { return ptr_ == rhs.ptr_; }

		/// Test for inequality with another iterator.
		bool operator !=(const ListIteratorBase& rhs) const { return ptr_ != rhs.ptr_; }

		/// Go to the next node.
		void GotoNext()
		{
			if (ptr_)
				ptr_ = ptr_->next_;
		}

		/// Go to the previous node.
		void GotoPrev()
		{
			if (ptr_)
				ptr_ = ptr_->prev_;
		}

		/// Node pointer.
		ListNodeBase* ptr_;
	};

	/// Doubly-linked list base class.
	class SAPPHIRE_API ListBase
	{
	public:
		/// Construct.
		ListBase() :
			allocator_(0),
			size_(0)
		{
		}

		/// Swap with another linked list.
		void Swap(ListBase& rhs)
		{
			Sapphire::Swap(head_, rhs.head_);
			Sapphire::Swap(tail_, rhs.tail_);
			Sapphire::Swap(allocator_, rhs.allocator_);
			Sapphire::Swap(size_, rhs.size_);
		}

	protected:
		/// Head node pointer.
		ListNodeBase* head_;
		/// Tail node pointer.
		ListNodeBase* tail_;
		/// Node allocator.
		AllocatorBlock* allocator_;
		/// Number of nodes.
		unsigned size_;
	};
}
