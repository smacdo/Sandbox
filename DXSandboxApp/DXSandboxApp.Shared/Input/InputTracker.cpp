#include "pch.h"
#include "InputTracker.h"

using namespace DXSandboxApp;

InputTracker::InputTracker()
    : mTracking(false),
      mPositionX(0.0f)
{
}

InputTracker::~InputTracker()
{
}

void InputTracker::StartTracking()
{
    mTracking = true;
}

void InputTracker::StopTracking()
{
    mTracking = false;
}

void InputTracker::TrackingUpdate(float positionX)
{
    mPositionX = positionX;
}
