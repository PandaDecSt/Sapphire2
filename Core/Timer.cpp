
#include "Predefined.h"

#include "CoreEvents.h"
#include "profiler.h"

#include <ctime>

#ifdef WIN32
#include <windows.h>
#include <mmsystem.h>
#elif __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif

#include "DebugNew.h"

namespace Sapphire
{

	bool HiresTimer::supported(false);
	long long HiresTimer::frequency(1000);

	Time::Time(Context* context) :
		Object(context),
		frameNumber_(0),
		timeStep_(0.0f),
		timerPeriod_(0)
	{
#ifdef WIN32
		LARGE_INTEGER frequency;
		if (QueryPerformanceFrequency(&frequency))
		{
			HiresTimer::frequency = frequency.QuadPart;
			HiresTimer::supported = true;
		}
#else
		HiresTimer::frequency = 1000000;
		HiresTimer::supported = true;
#endif
	}

	Time::~Time()
	{
		SetTimerPeriod(0);
	}

	static unsigned Tick()
	{
#ifdef WIN32
		return (unsigned)timeGetTime();
#elif __EMSCRIPTEN__
		return (unsigned)emscripten_get_now();
#else
		struct timeval time;
		gettimeofday(&time, NULL);
		return (unsigned)(time.tv_sec * 1000 + time.tv_usec / 1000);
#endif
	}

	static long long HiresTick()
	{
#ifdef WIN32
		if (HiresTimer::IsSupported())
		{
			LARGE_INTEGER counter;
			QueryPerformanceCounter(&counter);
			return counter.QuadPart;
		}
		else
			return timeGetTime();
#elif __EMSCRIPTEN__
		return (unsigned)(emscripten_get_now()*1000.0);
#else
		struct timeval time;
		gettimeofday(&time, NULL);
		return time.tv_sec * 1000000LL + time.tv_usec;
#endif
	}

	void Time::BeginFrame(float timeStep)
	{
		++frameNumber_;
		if (!frameNumber_)
			++frameNumber_;

		timeStep_ = timeStep;

		Profiler* profiler = GetSubsystem<Profiler>();
		if (profiler)
			profiler->BeginFrame();

		{
			SAPPHIRE_PROFILE(BeginFrame);

			// Frame begin event
			using namespace BeginFrame;

			VariantMap& eventData = GetEventDataMap();
			eventData[P_FRAMENUMBER] = frameNumber_;
			eventData[P_TIMESTEP] = timeStep_;
			SendEvent(E_BEGINFRAME, eventData);
		}
	}

	void Time::EndFrame()
	{
		{
			SAPPHIRE_PROFILE(EndFrame);

			// Frame end event
			SendEvent(E_ENDFRAME);
		}

		Profiler* profiler = GetSubsystem<Profiler>();
		if (profiler)
			profiler->EndFrame();
	}

	void Time::SetTimerPeriod(unsigned mSec)
	{
#ifdef WIN32
		if (timerPeriod_ > 0)
			timeEndPeriod(timerPeriod_);

		timerPeriod_ = mSec;

		if (timerPeriod_ > 0)
			timeBeginPeriod(timerPeriod_);
#endif
	}

	float Time::GetElapsedTime()
	{
		return elapsedTime_.GetMSec(false) / 1000.0f;
	}

	unsigned Time::GetSystemTime()
	{
		return Tick();
	}

	unsigned Time::GetTimeSinceEpoch()
	{
		return (unsigned)time(NULL);
	}

	String Time::GetTimeStamp()
	{
		time_t sysTime;
		time(&sysTime);
		const char* dateTime = ctime(&sysTime);
		return String(dateTime).Replaced("\n", "");
	}

	void Time::Sleep(unsigned mSec)
	{
#ifdef WIN32
		::Sleep(mSec);
#else
		usleep(mSec * 1000);
#endif
	}

	Timer::Timer()
	{
		Reset();
	}

	unsigned Timer::GetMSec(bool reset)
	{
		unsigned currentTime = Tick();
		unsigned elapsedTime = currentTime - startTime_;
		if (reset)
			startTime_ = currentTime;

		return elapsedTime;
	}

	void Timer::Reset()
	{
		startTime_ = Tick();
	}

	HiresTimer::HiresTimer()
	{
		Reset();
	}

	long long HiresTimer::GetUSec(bool reset)
	{
		long long currentTime = HiresTick();
		long long elapsedTime = currentTime - startTime_;

		// Correct for possible weirdness with changing internal frequency
		if (elapsedTime < 0)
			elapsedTime = 0;

		if (reset)
			startTime_ = currentTime;

		return (elapsedTime * 1000000LL) / frequency;
	}

	void HiresTimer::Reset()
	{
		startTime_ = HiresTick();
	}

}