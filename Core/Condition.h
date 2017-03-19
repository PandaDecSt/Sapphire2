#pragma once


#include "Sapphire.h"


namespace Sapphire
{

	// �߳̿ɷ�ȴ�������
	class SAPPHIRE_API Condition
	{
	public:
		
		Condition();

		~Condition();

		/// ��������
		void Set();

		/// �ȴ��������
		void Wait();

	private:
#ifndef WIN32
		/// �¼��������� ptheadsʵ�ֱ���
		void* mutex_;
#endif
		/// ����ϵͳ���ض��¼�
		void* event_;
	};
}