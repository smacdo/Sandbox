#include "stdafx.h"
#include <string>
#include "DXTestException.h"
#include "Utils.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sanbox engine exception base class.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SandboxException::SandboxException()
    : std::runtime_error("Unknown sandbox exception thrown"),
      mActionContext(),
      mMessage(L"Unspecified sandbox engine exception"),
      mFileName(L"N/A"),
      mLineNumber(0)
{
}

SandboxException::SandboxException(const char * pMessage)
    : std::runtime_error(pMessage),
      mActionContext(),
      mMessage(Utils::ConvertUtf8ToWString(pMessage)),
      mFileName(L"N/A"),
      mLineNumber(0)
{
}

SandboxException::SandboxException(const std::string& message)
    : std::runtime_error(message),
      mActionContext(),
      mMessage(Utils::ConvertUtf8ToWString(message)),
      mFileName(L"N/A"),
      mLineNumber(0)
{
}

SandboxException::SandboxException(const std::wstring& message)
    : std::runtime_error(Utils::ConvertUtf16ToUtf8(message)),
      mActionContext(),
      mMessage(message),
      mFileName(L"N/A"),
      mLineNumber(0)
{
}

SandboxException::SandboxException(const std::wstring& message, const std::wstring& actionContext)
    : std::runtime_error(Utils::ConvertUtf16ToUtf8(message)),
      mActionContext(actionContext),
      mMessage(message),
      mFileName(L"N/A"),
      mLineNumber(0)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// C standard library exception.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CStdLibException::CStdLibException(errno_t error)
    : SandboxException(Utils::GetErrorMessageFromErrno(error)),
      mErrorCode(error)
{
}

CStdLibException::CStdLibException(errno_t error, const std::wstring& actionContext)
    : SandboxException(Utils::GetErrorMessageFromErrno(error), actionContext),
      mErrorCode(error)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Windows API exception.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
WindowsApiException::WindowsApiException(unsigned long windowsApiErrorCode)
    : SandboxException(Utils::GetErrorMessageFromWinApiErrorCode(windowsApiErrorCode)),
      mWinApiErrorCode(windowsApiErrorCode)
{
}

WindowsApiException::WindowsApiException(unsigned long windowsApiErrorCode, const std::wstring& actionContext)
    : SandboxException(Utils::GetErrorMessageFromWinApiErrorCode(windowsApiErrorCode), actionContext),
      mWinApiErrorCode(windowsApiErrorCode)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DirectX exception.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DirectXException::DirectXException(unsigned long errorCode)
    : SandboxException(Utils::GetErrorMessageFromHResult(errorCode), L"Unexpected DirectX error"),
      mDirectXErrorCode(errorCode)
{
}

DirectXException::DirectXException(unsigned long errorCode, const std::wstring& actionContext)
    : SandboxException(Utils::GetErrorMessageFromHResult(errorCode), actionContext),
      mDirectXErrorCode(errorCode)
{
}

DirectXException::DirectXException(
    unsigned long errorCode,
    const std::wstring& actionContext,
    const std::wstring& fileName,
    int lineNumber)
    : SandboxException(Utils::GetErrorMessageFromHResult(errorCode), actionContext),
      mDirectXErrorCode(errorCode)
{
    mFileName = fileName;
    mLineNumber = lineNumber;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Assertion failed exception.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
AssertionFailedException::AssertionFailedException(const std::wstring& expression)
    : SandboxException(L"Assertion failed", expression)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// File load exception.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FileLoadException::FileLoadException(const std::wstring& filepath)
    : SandboxException(L"File could not be loaded", filepath)
{
}