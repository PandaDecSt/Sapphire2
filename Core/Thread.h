#pragma once


#include "Sapphire.h"


#ifndef WIN32
#include <pthread.h>
typedef pthread_t ThreadID;
#else
typedef unsigned ThreadID;
#endif

namespace Sapphire
{
	/// ����ϵͳ�̷߳�װ
	class SAPPHIRE_API Thread
	{
	public:
		/// ���캯�������Զ���ʼ�߳�
		Thread();
		/// �������������״̬��ֹͣ���ȴ��߳����
		virtual ~Thread();

		/// ����߳�Ҫ���еĺ���
		virtual void ThreadFunction() = 0;

		/// ��ʼ����̡߳�����ɹ�����true�����ʧ�ܿ����Ѿ����л���û�ܴ�������߳�
		bool Run();
		/// ���ñ�־λfalse�����ȴ��߳����
		void Stop();
		/// �����̵߳����ȼ�
		void SetPriority(int priority);

		/// ��������߳��ͷŴ���
		bool IsStarted() const { return handle_ != 0; }

		/// ���õ�ǰ�߳���Ϊ���߳�
		static void SetMainThread();
		/// ���ص�ǰ�߳�ID
		static ThreadID GetCurrentThreadID();
		/// �����ͷ������߳���ִ��
		static bool IsMainThread();

	protected:
		/// �߳̾��
		void* handle_;
		/// ���б�־
		volatile bool shouldRun_;

		/// ���̵߳��߳�ID
		static ThreadID mainThreadID;
	};
}

