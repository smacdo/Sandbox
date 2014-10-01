#pragma once
// TODO: Rename file to SandboxEngineExceptions.h
#include <exception>
#include <string>
#include <errno.h>

// TODO: Create an exception type macro.
// TODO: Make it so exceptions can store where the exception was originally thrown.

class SandboxException : public std::runtime_error
{
public:
    SandboxException();
    SandboxException(const char * pMessage);
    SandboxException(const std::string& message);
    SandboxException(const std::wstring& message);
    SandboxException(const std::wstring& message, const std::wstring& actionContext);

    std::wstring Message() const { return mMessage; }
    std::wstring ActionContext() const { return mActionContext; }
    std::wstring FileName() const { return mFileName; }
    int LineNumber() const { return mLineNumber; }

protected:
    std::wstring mActionContext;
	std::wstring mMessage;
    std::wstring mFileName;
    int mLineNumber;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Null pointer exception.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class NullPointerException : public SandboxException
{
public:
    NullPointerException(const std::wstring& variableName)
		: SandboxException(L"Value is null", variableName)
	{
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// C standard library exception.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CStdLibException : public SandboxException
{
public:
    CStdLibException(errno_t errorCode);
    CStdLibException(errno_t errorCode, const std::wstring& actionContext);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DirectX exception.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class DirectXException : public SandboxException
{
public:
    DirectXException(unsigned long errorCode);
    DirectXException(unsigned long errorCode, const std::wstring& actionContext);
    DirectXException(
        unsigned long errorCode, 
        const std::wstring& actionContext,
        const std::wstring& fileName,
        int lineNumber);
	
    unsigned long ErrorCode() const { return mDirectXErrorCode; }

protected:
    unsigned long mDirectXErrorCode;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Exception thrown resulting from error in a Windows API call.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class WindowsApiException : public SandboxException
{
public:
    WindowsApiException(unsigned long windowsApiErrorCode);
    WindowsApiException(unsigned long windowsApiErrorCode, const std::wstring& message);

    unsigned long ErrorCode() const
    {
        return mWinApiErrorCode;
    }

protected:
    unsigned long mWinApiErrorCode;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Assertion failed exception.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class AssertionFailedException : public SandboxException
{
public:
    AssertionFailedException(const std::wstring& expression);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// File load exception.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FileLoadException : public SandboxException
{
public:
    FileLoadException(const std::wstring& filepath);
};