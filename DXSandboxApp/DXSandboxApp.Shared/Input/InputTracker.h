#pragma once

namespace DXSandboxApp
{
	// This class is thread safe.
    class InputTracker
    {
    public:
        InputTracker();

		void PointerPressed(float x, float y);
		void PointerMoved(float x, float y);
		void PointerReleased(float x, float y);

		float InputPositionX() const;
		float InputPositionY() const;
		bool IsTracking() const;

	protected:
		void SetInputPosition(float x, float y);

    private:
        float mInputX;
		float mInputY;
        bool mTracking;
    };
}

