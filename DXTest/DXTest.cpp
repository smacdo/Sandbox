#include "DXSandbox.h"
#include "DXTestException.h"

#include <sstream>

// http://stackoverflow.com/a/377325/1922926
void VerifyDXResultImpl(long result, char * pFileName, int line, char * pMessage)
{
	if (result < 0)
	{
		std::stringstream ss;
		ss << pMessage << std::endl
			<< "FILE: " << pMessage << "(" << line << ")" << std::endl
			<< "HRESULT: " << result << std::endl;
			
		throw new DirectXException(ss.str().c_str());
	}
}

void VerifyCResult(errno_t error, const char* description)
{
	if (error != 0)
	{
		throw new CStdLibException(error);
	}
}

void ThrowExceptionIfFalse(bool result, const char * pExpression)
{
	if (!result)
	{
		throw new AssertionFailedException(pExpression);
	}
}

void ThrowExceptionIfNull(void* pointer, const char* pVariableName)
{
	if (pointer == nullptr)
	{
		throw new NullPointerException(pVariableName);
	}
}