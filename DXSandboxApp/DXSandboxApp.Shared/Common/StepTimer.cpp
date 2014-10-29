#include "pch.h"
#include "StepTimer.h"

using namespace DX;

/**
 * Default constructor.
 */
StepTimer::StepTimer()
    : mElapsedTimeUnits(0),
      mTotalTimeUnits(0),
      mLeftOverTimeUnits(0),
      mFrameCount(0),
      mFramesPerSecond(0),
      mFramesThisSecond(0),
      mQpcSecondCounter(0),
      mIsFixedTimeStep(false),
      mTargetElapsedTimeUnits(TimeUnitsPerSecond / 60)
{
    if (!QueryPerformanceFrequency(&mQpcFrequency))
    {
        throw ref new Platform::FailureException(L"Query performance frequency failed");
    }

    if (!QueryPerformanceCounter(&mQpcLastTime))
    {
        throw ref new Platform::FailureException(L"Query performance counter failed");
    }

    // Initialize max delta to 1/10 of a second.
    mQpcMaxDelta = mQpcFrequency.QuadPart / 10;
}

/**
 * Get elapsed time since the previous Update call.
 */
uint64 StepTimer::GetElapsedTimeUnits() const
{
    return mElapsedTimeUnits;
}

/**
 * Get elapsed time in seconds since the previous Update call.
 */
double StepTimer::GetElapsedSeconds() const
{
    return TimeUnitsToSeconds(mElapsedTimeUnits);
}

/**
 * Get total time since the start of the program.
 */
uint64 StepTimer::GetTotalTimeUnits() const
{
    return mTotalTimeUnits;
}

/**
 * Get total time in seconds since the start of the program.
 */
double StepTimer::GetTotalSeconds() const
{
    return TimeUnitsToSeconds(mTotalTimeUnits);
}

/**
 * Get total number of updates since start of the program.
 */
uint32 StepTimer::GetFrameCount() const
{
    return mFrameCount;
}

/**
 * Get the current frame rate.
 */
uint32 StepTimer::GetFramesPerSecond() const
{
    return mFramesPerSecond;
}

/**
 * Set whether to use fixed or variable timestep mode.
 */
void StepTimer::SetFixedTimeStep(bool isFixedTimestep)
{
    mIsFixedTimeStep = isFixedTimestep;
}

/**
 * Set how often to call Update when in fixed timestep mode.
 */
void StepTimer::SetTargetElapsedTimeUnits(uint64 targetElapsed)
{
    mTargetElapsedTimeUnits = targetElapsed;
}

void StepTimer::SetTargetElapsedSeconds(double targetElapsed)
{
    mTargetElapsedTimeUnits = SecondsToTimeUnits(targetElapsed);
}

double StepTimer::TimeUnitsToSeconds(uint64 TimeUnits)
{
    return static_cast<double>(TimeUnits) / TimeUnitsPerSecond;
}

uint64 StepTimer::SecondsToTimeUnits(double seconds)
{
    return static_cast<uint64>(seconds * TimeUnitsPerSecond);
}

// After an intentional timing discontinuity (for instance a blocking IO operation)
// call this to avoid having the fixed timestep logic attempt a set of catch-up 
// Update calls.
void StepTimer::ResetElapsedTime()
{
    if (!QueryPerformanceCounter(&mQpcLastTime))
    {
        throw ref new Platform::FailureException();
    }

    mLeftOverTimeUnits = 0;
    mFramesPerSecond = 0;
    mFramesThisSecond = 0;
    mQpcSecondCounter = 0;
}

uint64 StepTimer::PreUpdateCalculations()
{
    // Query the current time.
    LARGE_INTEGER currentTime;

    if (!QueryPerformanceCounter(&currentTime))
    {
        throw ref new Platform::FailureException();
    }

    uint64 timeDelta = currentTime.QuadPart - mQpcLastTime.QuadPart;

    mQpcLastTime = currentTime;
    mQpcSecondCounter += timeDelta;

    // Clamp excessively large time deltas (e.g. after paused in the debugger).
    if (timeDelta > mQpcMaxDelta)
    {
        timeDelta = mQpcMaxDelta;
    }

    // Convert QPC units into a canonical tick format. This cannot overflow due to the previous clamp.
    timeDelta *= TimeUnitsPerSecond;
    timeDelta /= mQpcFrequency.QuadPart;

    return timeDelta;
}

void StepTimer::PostUpdateCalculations(uint32 lastFrameCount)
{
    // Track the current framerate.
    if (mFrameCount != lastFrameCount)
    {
        mFramesThisSecond++;
    }

    if (mQpcSecondCounter >= static_cast<uint64>(mQpcFrequency.QuadPart))
    {
        mFramesPerSecond = mFramesThisSecond;
        mFramesThisSecond = 0;
        mQpcSecondCounter %= mQpcFrequency.QuadPart;
    }
}