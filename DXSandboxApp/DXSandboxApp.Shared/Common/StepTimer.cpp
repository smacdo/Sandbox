#include "pch.h"
#include "StepTimer.h"

#include <functional>

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

// Update timer state, calling the specified Update function the appropriate number of times.
void StepTimer::Update(std::function<void(double, double)> updateCallback)
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

    uint32 lastFrameCount = mFrameCount;

    if (mIsFixedTimeStep)
    {
        // Fixed timestep update logic

        // If the app is running very close to the target elapsed time (within 1/4 of a millisecond) just clamp
        // the clock to exactly match the target value. This prevents tiny and irrelevant errors
        // from accumulating over time. Without this clamping, a game that requested a 60 fps
        // fixed update, running with vsync enabled on a 59.94 NTSC display, would eventually
        // accumulate enough tiny errors that it would drop a frame. It is better to just round 
        // small deviations down to zero to leave things running smoothly.
        if (abs(static_cast<int64>(timeDelta - mTargetElapsedTimeUnits)) < TimeUnitsPerSecond / 4000)
        {
            timeDelta = mTargetElapsedTimeUnits;
        }

        mLeftOverTimeUnits += timeDelta;

        while (mLeftOverTimeUnits >= mTargetElapsedTimeUnits)
        {
            mElapsedTimeUnits = mTargetElapsedTimeUnits;
            mTotalTimeUnits += mTargetElapsedTimeUnits;
            mLeftOverTimeUnits -= mTargetElapsedTimeUnits;
            mFrameCount++;

            updateCallback(GetTotalSeconds(), GetElapsedSeconds());
        }
    }
    else
    {
        // Variable timestep update logic.
        mElapsedTimeUnits = timeDelta;
        mTotalTimeUnits += timeDelta;
        mLeftOverTimeUnits = 0;
        mFrameCount++;

        updateCallback(GetTotalSeconds(), GetElapsedSeconds());
    }

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