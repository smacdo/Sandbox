#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "Input.h"
#include "Graphics.h"

class Application
{
public:
	Application();
	~Application();

	void Initialize();
	void Run();
	void Shutdown();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	void Frame();		// again, terrible name.
	void InitializeWindows(int& screenWidth, int& screenHeight);	// TODO: change & and use std::pair<> instead.
	void ShutdownWindows();

private:
	LPCSTR mApplicationName;
	HINSTANCE mInstance;
	HWND mHwnd;

	Input * mpInput;
	Graphics * mpGraphics;

	bool mInitialized;
	bool mLoopAlive;
};

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
static Application * GApplication = 0;		// seriously?

