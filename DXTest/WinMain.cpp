#define WIN32_LEAN_AND_MEAN
#include "Application.h"
#include "DXTestException.h"
#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Application app;
	
	try
	{
		app.Initialize();
		app.Run();
		app.Shutdown();
	}
	catch (DXTestException& exception)
	{
		MessageBox(NULL, exception.what(), "Fatal Exception", MB_OK | MB_ICONSTOP | MB_SYSTEMMODAL | MB_SETFOREGROUND);
	}


	return 0;
}