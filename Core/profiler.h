#pragma once

#include "Str.h"
#include "Thread.h"
#include "Timer.h"


namespace Sapphire
{
	/// Profiling data for one block in the profiling tree.
	class SAPPHIRE_API ProfilerBlock
	{
	public:
		/// Construct with the specified parent block and name.
		ProfilerBlock(ProfilerBlock* parent, const char* name) :
			name_(0),
			time_(0),
			maxTime_(0),
			count_(0),
			parent_(parent),
			frameTime_(0),
			frameMaxTime_(0),
			frameCount_(0),
			intervalTime_(0),
			intervalMaxTime_(0),
			intervalCount_(0),
			totalTime_(0),
			totalMaxTime_(0),
			totalCount_(0)
		{
			if (name)
			{
				unsigned nameLength = String::CStringLength(name);
				name_ = new char[nameLength + 1];
				memcpy(name_, name, nameLength + 1);
			}
		}

		/// Destruct. Free the child blocks.
		~ProfilerBlock()
		{
			for (PODVector<ProfilerBlock*>::Iterator i = children_.Begin(); i != children_.End(); ++i)
			{
				delete *i;
				*i = 0;
			}

			delete[] name_;
		}

		/// Begin timing.
		void Begin()
		{
			timer_.Reset();
			++count_;
		}

		/// End timing.
		void End()
		{
			long long time = timer_.GetUSec(false);
			if (time > maxTime_)
				maxTime_ = time;
			time_ += time;
		}

		/// End profiling frame and update interval and total values.
		void EndFrame()
		{
			frameTime_ = time_;
			frameMaxTime_ = maxTime_;
			frameCount_ = count_;
			intervalTime_ += time_;
			if (maxTime_ > intervalMaxTime_)
				intervalMaxTime_ = maxTime_;
			intervalCount_ += count_;
			totalTime_ += time_;
			if (maxTime_ > totalMaxTime_)
				totalMaxTime_ = maxTime_;
			totalCount_ += count_;
			time_ = 0;
			maxTime_ = 0;
			count_ = 0;

			for (PODVector<ProfilerBlock*>::Iterator i = children_.Begin(); i != children_.End(); ++i)
				(*i)->EndFrame();
		}

		/// Begin new profiling interval.
		void BeginInterval()
		{
			intervalTime_ = 0;
			intervalMaxTime_ = 0;
			intervalCount_ = 0;

			for (PODVector<ProfilerBlock*>::Iterator i = children_.Begin(); i != children_.End(); ++i)
				(*i)->BeginInterval();
		}

		/// Return child block with the specified name.
		ProfilerBlock* GetChild(const char* name)
		{
			for (PODVector<ProfilerBlock*>::Iterator i = children_.Begin(); i != children_.End(); ++i)
			{
				if (!String::Compare((*i)->name_, name, true))
					return *i;
			}

			ProfilerBlock* newBlock = new ProfilerBlock(this, name);
			children_.Push(newBlock);

			return newBlock;
		}

		/// Block name.
		char* name_;
		/// High-resolution timer for measuring the block duration.
		HiresTimer timer_;
		/// Time on current frame.
		long long time_;
		/// Maximum time on current frame.
		long long maxTime_;
		/// Calls on current frame.
		unsigned count_;
		/// Parent block.
		ProfilerBlock* parent_;
		/// Child blocks.
		PODVector<ProfilerBlock*> children_;
		/// Time on the previous frame.
		long long frameTime_;
		/// Maximum time on the previous frame.
		long long frameMaxTime_;
		/// Calls on the previous frame.
		unsigned frameCount_;
		/// Time during current profiler interval.
		long long intervalTime_;
		/// Maximum time during current profiler interval.
		long long intervalMaxTime_;
		/// Calls during current profiler interval.
		unsigned intervalCount_;
		/// Total accumulated time.
		long long totalTime_;
		/// All-time maximum time.
		long long totalMaxTime_;
		/// Total accumulated calls.
		unsigned totalCount_;
	};

	/// Hierarchical performance profiler subsystem.
	class SAPPHIRE_API Profiler : public Object
	{
		SAPPHIRE_OBJECT(Profiler, Object);

	public:
		/// Construct.
		Profiler(Context* context);
		/// Destruct.
		virtual ~Profiler();

		/// Begin timing a profiling block.
		void BeginBlock(const char* name)
		{
			// Profiler supports only the main thread currently
			if (!Thread::IsMainThread())
				return;

			current_ = current_->GetChild(name);
			current_->Begin();
		}

		/// End timing the current profiling block.
		void EndBlock()
		{
			if (!Thread::IsMainThread())
				return;

			if (current_ != root_)
			{
				current_->End();
				current_ = current_->parent_;
			}
		}

		/// Begin the profiling frame. Called by HandleBeginFrame().
		void BeginFrame();
		/// End the profiling frame. Called by HandleEndFrame().
		void EndFrame();
		/// Begin a new interval.
		void BeginInterval();

		/// Return profiling data as text output.
		String PrintData(bool showUnused = false, bool showTotal = false, unsigned maxDepth = M_MAX_UNSIGNED) const;
		/// Return the current profiling block.
		const ProfilerBlock* GetCurrentBlock() { return current_; }
		/// Return the root profiling block.
		const ProfilerBlock* GetRootBlock() { return root_; }

	private:
		/// Return profiling data as text output for a specified profiling block.
		void PrintData(ProfilerBlock* block, String& output, unsigned depth, unsigned maxDepth, bool showUnused, bool showTotal) const;

		/// Current profiling block.
		ProfilerBlock* current_;
		/// Root profiling block.
		ProfilerBlock* root_;
		/// Frames in the current interval.
		unsigned intervalFrames_;
		/// Total frames.
		unsigned totalFrames_;
	};

	/// Helper class for automatically beginning and ending a profiling block
	class SAPPHIRE_API AutoProfileBlock
	{
	public:
		/// Construct. Begin a profiling block with the specified name and optional call count.
		AutoProfileBlock(Profiler* profiler, const char* name) :
			profiler_(profiler)
		{
			if (profiler_)
				profiler_->BeginBlock(name);
		}

		/// Destruct. End the profiling block.
		~AutoProfileBlock()
		{
			if (profiler_)
				profiler_->EndBlock();
		}

	private:
		/// Profiler.
		Profiler* profiler_;
	};

#ifdef SAPPHIRE_PROFILING
#define SAPPHIRE_PROFILE(name) Sapphire::AutoProfileBlock profile_ ## name (GetSubsystem<Sapphire::Profiler>(), #name)
#else
#define SAPPHIRE_PROFILE(name)
#endif
}