#pragma once

// Forward declarations.
namespace DX
{
    class StepTimer;
}

// Time profile categories that can be tracked.
enum DebugTimeStat
{
    Render,
    Update
};


// Tracks debugging information for the game.
class Profiler
{
public:
    Profiler();
    ~Profiler();

    void AddTimeMeasurement(DebugTimeStat stat, long long qpcTime);

private:
    void Init();
    double ConvertQpcToTime(long long qpcTime);

private:
    long long mQpcFrequency;
};

// Assists in tracking time statistics.
class ProfilerTimer sealed
{
public:
    ProfilerTimer(Profiler& profiler, DebugTimeStat statToTrack);
    ProfilerTimer(const ProfilerTimer&) = delete;
    ~ProfilerTimer();

    ProfilerTimer& operator = (const ProfilerTimer&) = delete;

private:
    Profiler& mProfiler;
    DebugTimeStat mStatToTrack;
    long long mStartTime;
};
