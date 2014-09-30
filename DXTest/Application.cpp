#include "Application.h"
#include "Input.h"
#include "Graphics.h"
#include "DXSandbox.h"
#include "DXTestException.h"

#include <cstdlib>      //  srand
#include <ctime>        // time

Application::Application()
: mApplicationName(nullptr),
  mInstance(nullptr),
  mHwnd(nullptr),
  mpInput(nullptr),
  mpGraphics(nullptr),
  mInitialized(false),
  mLoopAlive(false)
{
}


Application::~Application()
{
	if (!mInitialized)
	{
		Shutdown();
	}
}

void Application::Initialize()
{
	GApplication = this; // I hate tutorial code.
    srand((unsigned int) time(NULL));

	// Initialize app with windows.
	int screenWidth = 0, screenHeight = 0;
	InitializeWindows(screenWidth, screenHeight);

	// Create system objects.
	mpInput = new Input();
	mpInput->Initialize(mInstance, mHwnd, screenWidth, screenHeight);
	
	mpGraphics = new Graphics();
	mpGraphics->Initialize(screenWidth, screenHeight, mHwnd);

	mInitialized = true;
}

void Application::Run()
{
	MSG message;
	ZeroMemory(&message, sizeof(MSG));

	// Main game loop.
	mLoopAlive = true;

	while (mLoopAlive)
	{
		// Handle windows messages.
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		// Deal with quit message.
		if (message.message == WM_QUIT || mpInput->IsEscapePressed())
		{
			mLoopAlive = false;
		}
		else
		{
			Frame();
		}
	}
}

void Application::Frame()
{
    // Input processing.
    mpInput->Frame();

	// Quit?
    if (mpInput->IsEscapePressed())
	{
		mLoopAlive = false;
		return;
	}

	// Update systems.
	mpGraphics->Frame();
}

void Application::Shutdown()
{
	SafeDelete(mpGraphics);
	SafeDelete(mpInput);

	ShutdownWindows();

	mInitialized = false;
}

LRESULT CALLBACK Application::MessageHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

void Application::InitializeWindows(int& screenWidth, int& screenHeight)
{
	mInstance = GetModuleHandle(NULL);
	mApplicationName = "DirectX Tests by Scott";

	// Set up the windows class with default settings.
	WNDCLASSEX wc;

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = mInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = mApplicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	RegisterClassEx(&wc);

	// Determine rsolution of client's desktop screen.
	int posX = 0, posY = 0;

	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	if (FULL_SCREEN)
	{
		// NOT IMPLEMENTED.
		// http://rastertek.com/dx11tut02.html
	}
	else
	{
		screenWidth = 800;
		screenHeight = 600;

		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Create render window.
	mHwnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		mApplicationName,
		mApplicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX,
		posY,
		screenWidth,
		screenHeight,
		NULL,
		NULL,
		mInstance,
		NULL);

	if (mHwnd == nullptr)
	{
		throw WindowsApiException(GetLastError(), L"Failed to initialize main window");
	}

	// Bring the window up on the scren, and focus it.
	ShowWindow(mHwnd, SW_SHOW);
	SetForegroundWindow(mHwnd);
	SetFocus(mHwnd);

	// ShowCursor(false);
}

void Application::ShutdownWindows()
{
	// ShowCursor(true);

	// Fix display settings if we are leaving full screen mode.
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the rendering window.
	DestroyWindow(mHwnd);
	mHwnd = nullptr;

	// Remove the application instance.
	UnregisterClass(mApplicationName, mInstance);
	mInstance = nullptr;

	GApplication = nullptr;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
		
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	default:
		// dangerous if GApplication is NULL!!! this tutorial code sucks
		return GApplication->MessageHandler(hwnd, message, wparam, lparam);
	}
}