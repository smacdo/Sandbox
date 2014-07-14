#include "DXTestException.h"
#include <string>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// Code for this method: http://stackoverflow.com/a/455533/1922926
std::string WindowsApiException::GenerateMessage(long result, const std::string& message) const
{
	LPTSTR errorText = nullptr;
	std::string output;

	FormatMessage(
		  FORMAT_MESSAGE_FROM_SYSTEM		// use system message tables to retrieve error text
		| FORMAT_MESSAGE_ALLOCATE_BUFFER	// allocate buffer on local heap for error text
		| FORMAT_MESSAGE_IGNORE_INSERTS,	// Important! will fail otherwise, since we're not (and CANNOT) pass insertion parameters
		NULL,								// unused with FORMAT_MESSAGE_FROM_SYSTEM
		static_cast<HRESULT>(result),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&errorText,					// output 
		0,									// minimum size for output buffer
		NULL);								// arguments - see note 

	if (errorText != nullptr)
	{
		// ... do something with the string - log it, display it to the user, etc.
		output = message + std::string("(REASON: ") + std::string(errorText) + ")";

		// release memory allocated by FormatMessage()
		LocalFree(errorText);
		errorText = nullptr;
	}

	return output;
}