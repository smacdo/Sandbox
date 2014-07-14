#pragma once

#include "IInitializable.h"

#include <dinput.h>     // TODO: Eliminate.

/*
struct IDirectInput8;
struct IDirectInputDevice;
*/

class Input : public IInitializable
{
public:
	Input();
    Input(const Input& input) = delete;
	~Input();

    const Input& operator =(const Input& input) = delete;

    void Initialize(HINSTANCE appInstance, HWND hwnd, int screenWidth, int screenHeight);
    void Frame();

    bool IsEscapePressed() const;
    void GetMouseLocation(int *pX, int *pY) const;

    int MouseX() const { return mMouseX; }
    int MouseY() const { return mMouseY; }
    int MouseDeltaX() const { return mMouseDeltaX; }
    int MouseDeltaY() const { return mMouseDeltaY; }

private:
    void ReadKeyboard();
    void ReadMouse();
    void ProcessInput();

    virtual void OnShutdown() override;

private:
    bool mInitialized;
    IDirectInput8 * mpDirectInput;
    IDirectInputDevice8 * mpKeyboard;
    IDirectInputDevice8 * mpMouse;
    unsigned char mKeyboardState[256];
    int mScreenWidth, mScreenHeight;
    int mMouseX, mMouseY;
    int mMouseDeltaX, mMouseDeltaY;
};

