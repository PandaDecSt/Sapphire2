#pragma once

#include <Sapphire.h>

namespace Sapphire
{
	//应用计数
	struct RefCount
	{
		 
		RefCount() :
			refs_(0),
			weakRefs_(0)
		{
		}

		~RefCount()
		{
			//如果这个对象仍然能访问，设置引用小于0
			refs_ = -1;
			weakRefs_ = -1;
		}
		//引用
		int refs_;
		//弱引用
		int weakRefs_;

	};

	class SAPPHIRE_API RefCounted
	{
	public:
		//构造一个引用计数结构并初始设置一个自身弱引用
		RefCounted();
		//销毁，如果没有外部弱引用存在，使其失效并删除引用计数结构
		virtual ~RefCounted();

		/// Increment reference count. Can also be called outside of a SharedPtr for traditional reference counting.
		//  增加引用计数.
		void AddRef();
		/// Decrement reference count and delete self if no more references. Can also be called outside of a SharedPtr for traditional reference counting.
		//减少引用计数数量，如果没有更多的引用了删除对象。
		void ReleaseRef();
		/// Return reference count.
		// 返回引用计数
		int Refs() const;
		/// Return weak reference count.
		//返回弱引用数量
		int WeakRefs() const;

		/// Return pointer to the reference count structure.
		//  返回引用计数结构体的指针
		RefCount* RefCountPtr() { return refCount_; }

	private:
		/// Prevent copy construction.
		RefCounted(const RefCounted& rhs);
		/// Prevent assignment.
		RefCounted& operator =(const RefCounted& rhs);

		/// Pointer to the reference count structure.
		//指向引用计数结构体的指针
		RefCount* refCount_;
	};
}
