#pragma once 

#include "Sapphire.h"

namespace Sapphire
{

	struct AllocatorBlock;
	struct AllocatorNode;

	//�ڴ�����
	struct AllocatorBlock
	{
		//�ڵ��С
		unsigned nodeSize_;
		// ����ڴ��Ľڵ�����
		unsigned capacity_;
		//  ��һ�����еĽڵ�
		AllocatorNode* free_;
		// ��һ��������ָ��
		AllocatorBlock* next_;
	};

	//�ڴ����ڵ�
	struct AllocatorNode
	{
		/// ��һ�����нڵ�
		AllocatorNode* next_;
	};

	//��ָ���Ľڵ��С�ͳ�ʼ��������ʼ��һ���̶���С�ķ�����
	SAPPHIRE_API AllocatorBlock* AllocatorInitialize(unsigned nodeSize, unsigned initialCapacity = 1);
	//����ʼ��һ���̶���С�ķ��������ͷ���������������п�
	SAPPHIRE_API void AllocatorUninitialize(AllocatorBlock* allocator);
	//Ԥ��һ���ڵ㡣�����Ҫ����һ���¿�
	SAPPHIRE_API void* AllocatorReserve(AllocatorBlock* allocator);
	/// �ͷ�һ���ڵ㣬���ͷ��κο�
	SAPPHIRE_API void AllocatorFree(AllocatorBlock* allocator, void* ptr);

	// ������ģ���ࡣ
	template <class T> class Allocator
	{
	public:
		
		Allocator(unsigned initialCapacity = 0) :
			allocator_(0)
		{
			if (initialCapacity)
				allocator_ = AllocatorInitialize((unsigned)sizeof(T), initialCapacity);
		}

		~Allocator()
		{
			AllocatorUninitialize(allocator_);
		}

		//  ��Ĭ�ϵĹ�����Ԥ����һ������
		T* Reserve()
		{
			if (!allocator_)
				allocator_ = AllocatorInitialize((unsigned)sizeof(T));
			T* newObject = static_cast<T*>(AllocatorReserve(allocator_));
			new(newObject)T();

			return newObject;
		}

		// �ø��ƹ�������Ԥ����һ������
		T* Reserve(const T& object)
		{
			if (!allocator_)
				allocator_ = AllocatorInitialize((unsigned)sizeof(T));
			T* newObject = static_cast<T*>(AllocatorReserve(allocator_));
			new(newObject)T(object);

			return newObject;
		}

		// ���ٺ��ͷ�һ������
		void Free(T* object)
		{
			(object)->~T();
			AllocatorFree(allocator_, object);
		}

	private:

		Allocator(const Allocator<T>& rhs);

		Allocator<T>& operator =(const Allocator<T>& rhs);

		// �ڴ�����ָ��
		AllocatorBlock* allocator_;
	};
}