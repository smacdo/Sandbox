#define WIN32_LEAN_AND_MEAN
#include "Application.h"
#include "DXTestException.h"
#include <Windows.h>
#include <sstream>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Application app;
	
	try
	{
		app.Initialize();
		app.Run();
		app.Shutdown();
	}
	catch (SandboxException& exception)
	{
        // Format the message that we want to display to the user.
        std::wostringstream ss;

        ss 
            << L"MESSAGE: " << exception.Message() << std::endl
            << L"CONTEXT: " << exception.ActionContext() << std::endl
            << L"FILE: " << exception.FileName() << std::endl
            << L"LINE: " << exception.LineNumber();

        // Show the error message.
		MessageBoxW(
            NULL,
            ss.str().c_str(),
            L"Fatal Exception",
            MB_OK | MB_ICONSTOP | MB_SYSTEMMODAL | MB_SETFOREGROUND);
	}


	return 0;
}