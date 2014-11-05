#include "pch.h"
#include "Profiler.h"
#include "Common/StepTimer.h"

using namespace DX;

Profiler::Profiler()
    : mQpcFrequency(0)
{
}

Profiler::~Profiler()
{
}

void Profiler::Init()
{
    // Query performance frequency.
    LARGE_INTEGER qpcFrequency;

    if (!QueryPerformanceFrequency(&qpcFrequency))
    {
        throw ref new Platform::FailureException(L"Query performance frequency failed");
    }

    mQpcFrequency = qpcFrequency.QuadPart;
}

// Adds a time measurement to the profiler.
void Profiler::AddTimeMeasurement(DebugTimeStat stat, long long time)
{

}

double Profiler::ConvertQpcToTime(long long qpcTime)
{
    return static_cast<double>(qpcTime) / static_cast<double>(mQpcFrequency);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Profiler timer
///////////////////////////////////////////////////////////////////////////////////////////////////
ProfilerTimer::ProfilerTimer(Profiler& profiler, DebugTimeStat statToTrack)
    : mProfiler(profiler),
      mStatToTrack(statToTrack),
      mStartTime(0)
{
    LARGE_INTEGER currentTime;

    if (!QueryPerformanceCounter(&currentTime))
    {
        throw ref new Platform::FailureException();
    }

    mStartTime = currentTime.QuadPart;
}

ProfilerTimer::~ProfilerTimer()
{
    // Query the current time.
    LARGE_INTEGER currentTime;

    if (!QueryPerformanceCounter(&currentTime))
    {
        throw ref new Platform::FailureException();
    }

    mProfiler.AddTimeMeasurement(mStatToTrack, currentTime.QuadPart - mStartTime);
}