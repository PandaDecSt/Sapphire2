#pragma once

#include "Sapphire.h"


namespace Sapphire
{
	/// ������
	class SAPPHIRE_API Mutex
	{
	public:
		Mutex();
		~Mutex();

		//  �õ������壬����Ѿ����õ���������
		void Acquire();
		/// �ͷŻ�����
		void Release();

	private:
		/// ��������
		void* handle_;
	};

	// �Զ���ú��ͷŻ��������
	class SAPPHIRE_API MutexLock
	{
	public:
		/// ���첢��û�����
		MutexLock(Mutex& mutex);
		/// �������ͷŻ�����
		~MutexLock();

	private:
		/// ��ֹ�������캯��
		MutexLock(const MutexLock& rhs);
		/// ��ֹ��ֵ
		MutexLock& operator =(const MutexLock& rhs);

		/// ����������
		Mutex& mutex_;
	};
}