#pragma once

#include <wrl.h>
#include <functional>

namespace DX
{
	// Helper class for animation and simulation timing.
	class StepTimer
	{
	public:
        StepTimer();

		// Get elapsed time since the previous Update call.
        uint64 GetElapsedTimeUnits() const;
        double GetElapsedSeconds() const;

		// Get total time since the start of the program.
        uint64 GetTotalTimeUnits() const;
        double GetTotalSeconds() const;

		// Get total number of updates since start of the program.
        uint32 GetFrameCount() const;

		// Get the current framerate.
        uint32 GetFramesPerSecond() const;

		// Set whether to use fixed or variable timestep mode.
        void SetFixedTimeStep(bool isFixedTimestep);

		// Set how often to call Update when in fixed timestep mode.
        void SetTargetElapsedTimeUnits(uint64 targetElapsed);
        void SetTargetElapsedSeconds(double targetElapsed);

		// Integer format represents time using 10,000,000 TimeUnits per second.
		static const uint64 TimeUnitsPerSecond = 10000000;

        static double TimeUnitsToSeconds(uint64 TimeUnits);
        static uint64 SecondsToTimeUnits(double seconds);

		// After an intentional timing discontinuity (for instance a blocking IO operation)
		// call this to avoid having the fixed timestep logic attempt a set of catch-up 
		// Update calls.
        void ResetElapsedTime();

		// Update timer state, calling the specified Update function the appropriate number of times.
        void Update(std::function<void(double, double)> updateCallback);

    private:

	private:
		// Source timing data uses QPC units.
		LARGE_INTEGER mQpcFrequency;
		LARGE_INTEGER mQpcLastTime;
		uint64 mQpcMaxDelta;

		// Derived timing data uses a canonical tick format.
		uint64 mElapsedTimeUnits;
		uint64 mTotalTimeUnits;
		uint64 mLeftOverTimeUnits;

		// Members for tracking the framerate.
		uint32 mFrameCount;
		uint32 mFramesPerSecond;
		uint32 mFramesThisSecond;
		uint64 mQpcSecondCounter;

		// Members for configuring fixed timestep mode.
		bool mIsFixedTimeStep;
		uint64 mTargetElapsedTimeUnits;
	};
}
