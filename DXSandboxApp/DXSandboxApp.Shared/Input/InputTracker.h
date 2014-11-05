#pragma once

namespace DXSandboxApp
{
    class InputTracker
    {
    public:
        InputTracker();
        ~InputTracker();

        float InputPositionX() const { return mPositionX; }

        void StartTracking();
        void StopTracking();
        bool IsTracking() const { return mTracking; }

        void TrackingUpdate(float positionX);

    private:
        float mPositionX;
        bool mTracking;
    };
}

