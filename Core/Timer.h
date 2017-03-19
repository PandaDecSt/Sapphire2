#pragma once 

#include "Object.h"


namespace Sapphire
{
	/// �ͷֱ��ʲ���ϵͳ��ʱ��
	class SAPPHIRE_API Timer
	{
	public:
		/// ���캯������ȡ��ʼʱ��ֵ
		Timer();

		/// ��ȡ��ȥ�ĺ��������ҿ���ѡ������
		unsigned GetMSec(bool reset);
		/// ���ü�ʱ��
		void Reset();

	private:
		/// ��ʼ��ʱ�Ӻ�����
		unsigned startTime_;
	};

	/// �������ܷ����ĸ߷ֱ��ʲ���ϵͳ��ʱ��
	class SAPPHIRE_API HiresTimer
	{
		friend class Time;

	public:
		/// ��ȡ��ʼ�ĸ߷ֱ��ʵ�ʱ��ֵ
		HiresTimer();

		/// �������ŵĺ��������ҿ���������
		long long GetUSec(bool reset);
		/// ���ü�ʱ��
		void Reset();

		/// �����Ƿ�֧�ָ߷ֱ��ʼ�ʱ��
		static bool IsSupported() { return supported; }

		/// ���֧�֣����ظ߷ֱ��ʼ�ʱ����Ƶ��
		static long long GetFrequency() { return frequency; }

	private:
		/// ��ʼ��ʱ��ֵCPU��ticks
		long long startTime_;

		/// �Ƿ�֧�ָ߷ֱ��ʼ�ʱ���ı�־
		static bool supported;
		/// �߷ֱ��ʼ�ʱ����Ƶ��
		static long long frequency;
	};

	///ʱ���֡������ϵͳ
	class SAPPHIRE_API Time : public Object
	{
		SAPPHIRE_OBJECT(Time, Object);

	public:
		
		Time(Context* context);
		/// ��������������˵ͷֱ��ʼ�ʱ��������
		virtual ~Time();

		/// ����һ֡�ĳ���ʱ����������ʼ�µ�һ֡�����ҷ���֡��ʼ�¼�
		void BeginFrame(float timeStep);
		/// ����֡�� ������ʱ�䲢�ҷ���֡�����¼�
		void EndFrame();
		/// ���ð�����ĵͷֱ��ʼ�ʱ���� 0���赽Ĭ��ʱ��
		void SetTimerPeriod(unsigned mSec);

		///��ȡ֡���� ��BeginFram()��ʼ���ú�Ϊ1֡
		unsigned GetFrameNumber() const { return frameNumber_; }

		/// ���ص�ǰ֡����ʱ�䲽��
		float GetTimeStep() const { return timeStep_; }

		/// ��ȡ��ǰ�ͷֱ��ʼ�ʱ������ʱ������
		unsigned GetTimerPeriod() const { return timerPeriod_; }

		/// ���شӳ���ʼʱ��ȥ��ʱ��
		float GetElapsedTime();

		/// ��ȡϵͳʱ�����
		static unsigned GetSystemTime();
		/// ��ȡ��1970/1/1��ʼ��ϵͳʱ����
		static unsigned GetTimeSinceEpoch();
		/// ��ȡ����/ʱ����Ϊһ���ַ���
		static String GetTimeStamp();
		/// ����һ���ĺ�����
		static void Sleep(unsigned mSec);

	private:
		/// ����ʼʱ��ȥ��ʱ��
		Timer elapsedTime_;
		/// ֡��
		unsigned frameNumber_;
		/// ��Ϊ��λ��ʱ�䲽��
		float timeStep_;
		/// �ͷֱ��ʼ�ʱ������
		unsigned timerPeriod_;
	};
}