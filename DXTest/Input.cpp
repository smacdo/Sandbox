#include "Input.h"
#include "DXTest.h"

#include <dinput.h>
#include <algorithm>

#undef min      // TODO: better way?
#undef max

// DirectInput compiler directives. Make sure to pull in v8 to avoid warnings, and tell the linker to link
// against the DirectInput libraries.
#define DIRECTINPUT_VERSION 0x800
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

Input::Input()
: IInitializable(),
  mpDirectInput(nullptr),
  mpKeyboard(nullptr),
  mpMouse(nullptr),
  mKeyboardState(),
  mScreenWidth(0),
  mScreenHeight(0),
  mMouseX(0),
  mMouseY(0)
{
}

Input::~Input()
{
}

void Input::Initialize(HINSTANCE appInstance, HWND hwnd, int screenWidth, int screenHeight)
{
    if (!IsInitialized())
    {
        mScreenWidth = screenWidth;
        mScreenHeight = screenHeight;
        mMouseX = 0;
        mMouseY = 0;

        // Call out to DirectInput and get a pointer to the newly created DInput interface.
        HRESULT result = DirectInput8Create(appInstance,
                                            DIRECTINPUT_VERSION,
                                            IID_IDirectInput8,
                                            reinterpret_cast<void**>(&mpDirectInput),
                                            nullptr);

        VerifyDXResult(result);
        VerifyNotNull(mpDirectInput);

        // Create and initialize the keyboard input interface.
        result = mpDirectInput->CreateDevice(GUID_SysKeyboard, &mpKeyboard, nullptr);

        VerifyDXResult(result);
        VerifyNotNull(mpKeyboard);

        result = mpKeyboard->SetDataFormat(&c_dfDIKeyboard);
        VerifyDXResult(result);

        result = mpKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
        VerifyDXResult(result);

        result = mpKeyboard->Acquire();
        VerifyDXResult(result);

        // Create and initialize the mouse input.
        result = mpDirectInput->CreateDevice(GUID_SysMouse, &mpMouse, nullptr);

        VerifyDXResult(result);
        VerifyNotNull(mpMouse);

        result = mpMouse->SetDataFormat(&c_dfDIMouse);
        VerifyDXResult(result);

        result = mpMouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
        VerifyDXResult(result);

        result = mpMouse->Acquire();
        VerifyDXResult(result);
     
        // All done!
        SetInitialized();
    }
}

void Input::OnShutdown()
{
    if (mpMouse != nullptr)
    {
        mpMouse->Unacquire();
        SafeRelease(mpMouse);
    }

    if (mpKeyboard != nullptr)
    {
        mpKeyboard->Unacquire();
        SafeRelease(mpKeyboard);
    }

    SafeRelease(mpDirectInput);
}

void Input::Frame()
{
    ReadKeyboard();
    ReadMouse();
    ProcessInput();
}

// Read and copy the state of the keyboard into the mKeyboardState variable. This state will reflect the state of all
// keys, and if they are pressed/not pressed.
//
// If the reading fails this method will attempt to recover from common error states (like being unaquired) before
// throwing an exception.
void Input::ReadKeyboard()
{
    HRESULT result = mpKeyboard->GetDeviceState(sizeof(mKeyboardState), (LPVOID)&mKeyboardState);
    
    if (FAILED(result))
    {
        // Check if the keyboard lost focus or was unaquired. If so, try to get control back before raising an
        // exception.
        if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED)
        {
            result = mpKeyboard->Acquire();
        }

//        VerifyDXResult(result);
    }
}

// Read and copy the state of the mouse. Track both the mouse movement delta as well as the absolute position of the
// mouse. Also, this method will attempt to recover from common input error conditions similiar to ReadKeyboard().
void Input::ReadMouse()
{
    DIMOUSESTATE mouseState;
    HRESULT result = mpMouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mouseState);

    if (FAILED(result))
    {
        // Check if the mouse lost focus or was unaquired. If so, try to get control back before raising an exception.
        if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED)
        {
            result = mpKeyboard->Acquire();
        }

//        VerifyDXResult(result);
    }

    mMouseDeltaX = mouseState.lX;
    mMouseDeltaY = mouseState.lY;
}

// Process the changes that have happend in the input devices since the last frame, and update the input manager's
// state.
void Input::ProcessInput()
{
    // Update position of the mouse cursor based on the change of the mouse location during the frame.
    mMouseX += mMouseDeltaX;
    mMouseY += mMouseDeltaY;

    // Clamp mouse position to screen size.
    mMouseX = std::max(std::min(mMouseX, mScreenWidth), 0);
    mMouseY = std::max(std::min(mMouseY, mScreenHeight), 0);
}

// Check if escape button is pressed.
bool Input::IsEscapePressed() const
{
    return (mKeyboardState[DIK_ESCAPE] & 0x80);
}

void Input::GetMouseLocation(int * pX, int * pY) const
{
    if (pX != nullptr)
    {
        *pX = MouseX();
    }

    if (pY != nullptr)
    {
        *pY = MouseY();
    }
}