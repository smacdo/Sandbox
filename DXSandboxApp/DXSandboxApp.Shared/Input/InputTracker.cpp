#include "pch.h"
#include "InputTracker.h"

using namespace DXSandboxApp;

InputTracker::InputTracker()
    : mTracking(false),
      mInputX(0.0f),
	  mInputY(0.0f)
{
}

void InputTracker::PointerPressed(float x, float y)
{
	mTracking = true;
	SetInputPosition(x, y);
}

void InputTracker::PointerMoved(float x, float y)
{
	SetInputPosition(x, y);
}

void InputTracker::PointerReleased(float x, float y)
{
	mTracking = false;
	SetInputPosition(x, y);
}

float InputTracker::InputPositionX() const
{
	return mInputX;
}

float InputTracker::InputPositionY() const
{
	return mInputY;
}

bool InputTracker::IsTracking() const
{
	return mTracking;
}

void InputTracker::SetInputPosition(float x, float y)
{
	// We can only exchange 32 bit values as LONG, so need to do some reinterpret magic.
	//  Minor bug: It is possible that another thread may aquire an updated x value but not the updated y value if it
	//             reads prior to completion of this function.
	InterlockedExchange(
		reinterpret_cast<LONG*>(&mInputX),
		*reinterpret_cast<LONG*>(&x));

	InterlockedExchange(
		reinterpret_cast<LONG*>(&mInputY),
		*reinterpret_cast<LONG*>(&y));
}