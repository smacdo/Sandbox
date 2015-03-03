#include "pch.h"
#include "Utility.h"

#include <string>
#include <windows.h>

using namespace DXSandboxApp;

inline void DebugPrint(const std::wstring& text)
{
	std::wstring output = text + L"\n";
	OutputDebugStringW(output.c_str());
}