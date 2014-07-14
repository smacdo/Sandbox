#pragma once
#include <exception>
#include <string>
#include <errno.h>

class DXTestException : public std::exception
{
public:
	DXTestException()
		: mMessage("Unspecified DXTest exception")
	{
	}

	DXTestException(const char * pMessage)
		: mMessage(pMessage)
	{
	}

	DXTestException(const std::string& message)
		: mMessage(message)
	{
	}

	DXTestException(const DXTestException& exception)
		: mMessage(exception.mMessage)
	{
	}

	virtual ~DXTestException()
	{
	}

	DXTestException& operator= (const DXTestException& exception)
	{
		SetMessage(exception.Message());
	}

	virtual const char* what() const
	{
		return mMessage.c_str();
	}

	std::string Message() const
	{
		return mMessage;
	}

protected:
	void SetMessage(const std::string& message)
	{
		mMessage = message;
	}

	void SetMessage(const char* pMessage)
	{
		mMessage = pMessage;
	}

private:
	std::string mMessage;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Null pointer exception.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class NullPointerException : public DXTestException
{
public:
	NullPointerException(const char * pVariableName)
		: DXTestException(std::string(pVariableName) + std::string(" is null"))
	{
	}

	NullPointerException(const NullPointerException& exception)
		: DXTestException(exception)
	{
	}

	virtual ~NullPointerException()
	{
	}

	NullPointerException& operator= (const NullPointerException& exception)
	{
		SetMessage(exception.Message());
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// C standard library exception.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CStdLibException : public DXTestException
{
public:
	CStdLibException(errno_t error)
		: DXTestException("C standard library call failed, need to add code to get error value")
	{
	}

	CStdLibException(const NullPointerException& exception)
		: DXTestException(exception)
	{
	}

	virtual ~CStdLibException()
	{
	}

	CStdLibException& operator= (const CStdLibException& exception)
	{
		SetMessage(exception.Message());
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DirectX exception.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class DirectXException : public DXTestException
{
public:
	DirectXException(const char * pMessage)
		: DXTestException(pMessage)
	{
	}

	DirectXException(const DirectXException& exception)
		: DXTestException(exception)
	{
	}

	virtual ~DirectXException()
	{
	}

	DirectXException& operator= (const DirectXException& exception)
	{
		SetMessage(exception.Message());
	}

protected:
	std::string GenerateMessage(const std::string& message) const;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Exception thrown resulting from error in a Windows API call.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class WindowsApiException : public DXTestException
{
public:
	WindowsApiException(long result, const char * pMessage)
		: DXTestException(pMessage)
	{
	}

	WindowsApiException(const char *pMessage)
		: DXTestException(pMessage)
	{
	}

	WindowsApiException(const WindowsApiException& exception)
		: DXTestException(exception)
	{
	}

	virtual ~WindowsApiException()
	{
	}

	WindowsApiException& operator= (const WindowsApiException& exception)
	{
		SetMessage(exception.Message());
	}

protected:
	std::string GenerateMessage(long result, const std::string& message) const;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Assertion failed exception.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class AssertionFailedException : public DXTestException
{
public:
	AssertionFailedException(const char * pMessage)
		: DXTestException(pMessage)
	{
	}

	AssertionFailedException(const AssertionFailedException& exception)
		: DXTestException(exception)
	{
	}

	virtual ~AssertionFailedException()
	{
	}

	AssertionFailedException& operator= (const AssertionFailedException& exception)
	{
		SetMessage(exception.Message());
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Null pointer exception.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FileLoadException : public DXTestException
{
public:
	FileLoadException(const std::string& filepath)
		: DXTestException(filepath + std::string(" could not be loaded"))
	{
	}

	FileLoadException(const FileLoadException& exception)
		: DXTestException(exception)
	{
	}

	virtual ~FileLoadException()
	{
	}

	FileLoadException& operator= (const FileLoadException& exception)
	{
		SetMessage(exception.Message());
	}
};