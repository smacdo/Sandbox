#include "stdafx.h"
#include "Utils.h"
#include "DXTestException.h"

#include <algorithm>        // string trimming
#include <cctype>           // string trimming
#include <functional>       // string trimming

using namespace Utils;

// http://stackoverflow.com/a/6691829
//  TODO: Test MB_ERR_INVALID_CHARS failure
std::wstring Utils::ConvertUtf8ToWString(const std::string& input)
{
	// Deal with the trivial case of being passed an empty string.
	if (input.empty())
	{
		return std::wstring();
	}

	// Determine the length required to hold the converted wstring.
    int requiredLength = ::MultiByteToWideChar(
        CP_UTF8,
        MB_ERR_INVALID_CHARS,
        input.c_str(),
        static_cast<int>(input.length() * sizeof(char)),
        0,
        0);

    if (requiredLength == 0)
    {
        throw WindowsApiException(GetLastError(), L"ConvertUtf8ToWString - first pass");
    }

	// Presize an empty wstring and convert the UTF8 string to wstring in place.
	std::wstring output(requiredLength, L'\0');
	int result = ::MultiByteToWideChar(
        CP_UTF8,
        MB_ERR_INVALID_CHARS,
        input.c_str(),
        static_cast<int>(input.length()),
        &output[0],
        static_cast<int>(output.length()));

    if (result == 0)
    {
        throw WindowsApiException(GetLastError(), L"ConvertUtf8ToWString - second pass");
    }

	return output;
}

// TODO: Test MB_ERR_INVALID_CHARS and WC_NO_BEST_FIT_CHARS (look at docs)
std::string Utils::ConvertUtf16ToUtf8(const std::wstring& input)
{
    // Deal with the trivial case of being passed an empty string.
    if (input.empty())
    {
        return std::string();
    }

    // Determine the length required to hold the converted string.
    int requiredLength = ::WideCharToMultiByte(
        CP_UTF8,
        WC_ERR_INVALID_CHARS,
        input.c_str(),
        static_cast<int>(input.length()),
        0,          // pointer to buffer that receives the converted string.
        0,          // size in bytes of the buffer above.
        nullptr,    // default character if converted char cannot be represented (must be null for UTF8).
        nullptr);   // pointer to flag that indicates default character used. (must be null for UTF8).

    if (requiredLength == 0)
    {
        throw WindowsApiException(GetLastError(), L"ConvertUtf16ToUtf8 - first pass");
    }

    // Presize an empty string and convert the wide string to a UTF8 string in place.
    std::string output(requiredLength, '\0');
    int result = ::WideCharToMultiByte(
        CP_UTF8,
        WC_ERR_INVALID_CHARS,
        input.c_str(),                                      // wide string pointer.
        static_cast<int>(input.length()),                   // wide string length in characters.
        &output[0],                                         // narrow output string pointer.
        static_cast<int>(output.length() * sizeof(char)),   // narrow output string length in bytes.
        nullptr,                                            // default character for invalid converted chars.
        nullptr);                                           // pointer to flag that is set if default char is used.

    if (result == 0)
    {
        throw WindowsApiException(GetLastError(), L"ConvertUtf16ToUtf8 - second pass");
    }

    return output;
}

// Code for this method: http://stackoverflow.com/a/455533/1922926
//  TODO: Test this.
std::wstring Utils::GetErrorMessageFromHResult(unsigned long errorCode)
{
    return GetErrorMessageFromWinApiErrorCode(errorCode);
}

///
std::wstring Utils::GetErrorMessageFromWinApiErrorCode(unsigned long windowsApiErrorCode)
{
    LPTSTR errorText = nullptr;
    std::wstring output;

    DWORD result = FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM		    // Use system message tables to retrieve error text.
        | FORMAT_MESSAGE_ALLOCATE_BUFFER	// Allocate buffer on local heap for error text.
        | FORMAT_MESSAGE_IGNORE_INSERTS,	// Do not use message formattion parameters.
        nullptr,							// Unused with FORMAT_MESSAGE_FROM_SYSTEM
        windowsApiErrorCode,                // Convert error code from hresult.
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),  // Use the standard neutral system language. (Always english?)
        (LPTSTR) &errorText,                // Output buffer pointer.
        0,									// The minimum size to allocate for the output string buffer.
        nullptr);							// No passing of message formatting parameters.

    if (result == 0)
    {
        DWORD actualError = GetLastError();
        output = L"Failed to get error code message";
    }
    else if (errorText != nullptr)
    {
        // Copy temporary buffer allocate by windows into the output string, and then release the windows
        // allocated string buffer.
        output = std::wstring(errorText);

        LocalFree(errorText);
        errorText = nullptr;
    }
    else
    {
        output = L"Failed to get error code message, returned pointer was null";
    }

    return output;
}

std::wstring Utils::GetErrorMessageFromErrno(errno_t errorCode)
{
    const size_t MaxBufferSize = 1024;
    std::wstring output(MaxBufferSize, L'\0');

    errno_t result = _wcserror_s(&output[0], MaxBufferSize, errorCode);

    if (result != 0)
    {
        return L"Failed to get error code message";
    }
    else
    {
        output.resize(output.find_first_of(L'\0'));
        return output;
    }
}

bool Utils::StartsWith(const std::string& fullString, const std::string& prefix)
{
    bool result = false;

    if (!prefix.empty() && fullString.length() >= prefix.length())
    {
        result = (0 == fullString.compare(0, prefix.size(), prefix));
    }

    return result;
}

bool Utils::StartsWith(const std::wstring& fullString, const std::wstring& prefix)
{
    bool result = false;

    if (!prefix.empty() && fullString.length() >= prefix.length())
    {
        result = (0 == fullString.compare(0, prefix.size(), prefix));
    }

    return result;
}

// http://stackoverflow.com/a/874160
bool Utils::EndsWith(const std::string& fullString, const std::string& ending)
{
    bool result = false;

    if (!ending.empty() && fullString.length() >= ending.length())
    {
        result = (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
    }

    return result;
}

// http://stackoverflow.com/a/874160
bool Utils::EndsWith(const std::wstring& fullString, const std::wstring& ending)
{
    bool result = false;

    if (!ending.empty() && fullString.length() >= ending.length())
    {
        result = (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// string trimming functions
// [+29] [2009-01-22 16:00:07] Evan Teran 
// http://www.stackprinter.com/export?service=stackoverflow&question=469696
///////////////////////////////////////////////////////////////////////////////////////////////////
std::string Utils::RightTrim(const std::string& text)
{
    std::string s(text);
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

std::wstring Utils::RightTrim(const std::wstring& text)
{
    std::wstring s(text);
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

std::string Utils::LeftTrim(const std::string& text)
{
    std::string s(text);
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

std::wstring Utils::LeftTrim(const std::wstring& text)
{
    std::wstring s(text);
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

std::string Utils::Trim(const std::string& text)
{
    std::string s(text);
    return LeftTrim(RightTrim(s));
}

std::wstring Utils::Trim(const std::wstring& text)
{
    std::wstring s(text);
    return LeftTrim(RightTrim(s));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Random
///////////////////////////////////////////////////////////////////////////////////////////////////
// http://stackoverflow.com/a/686373
float Utils::RandFloat()
{
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

float Utils::RandFloat(float min, float max)
{
    float random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    float difference = min - max;
    float r = random * difference;

    return min + r;
}
