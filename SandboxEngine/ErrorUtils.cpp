#include "stdafx.h"

#include "DXSandbox.h"
#include "Utils.h"
#include "DXTestException.h"

#include <sstream>

// http://stackoverflow.com/a/377325/1922926
void VerifyDXResultImpl(
    unsigned long result,
    const char * fileName,
    int line,
    const char * actionContext)
{
	if (result < 0)
	{
        throw DirectXException(
            result,
            Utils::ConvertUtf8ToWString(actionContext),
            Utils::ConvertUtf8ToWString(fileName),
            line);
	}
}

void VerifyCResult(errno_t error, const char* description)
{
	if (error != 0)
	{
		throw CStdLibException(error);
	}
}

void ThrowExceptionIfFalse(bool result, const char * expression)
{
	if (!result)
	{
        throw AssertionFailedException(Utils::ConvertUtf8ToWString(expression));
	}
}

void ThrowExceptionIfNull(void* pointer, const char* variableName)
{
	if (pointer == nullptr)
	{
        throw NullPointerException(Utils::ConvertUtf8ToWString(variableName));
	}
}