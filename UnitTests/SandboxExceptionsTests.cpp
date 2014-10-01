#include "stdafx.h"
#include "CppUnitTest.h"
#include "DXTestException.h"

#include <string>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{
    // Unit tests for my extensions to SimpleMath.h
    TEST_CLASS(SandboxExceptionsTests)
    {
    public:
        TEST_METHOD(CreateCErrnoException)
        {
            // ERANGE.
            CStdLibException ex(EDOM, L"context");

            Assert::AreEqual((errno_t) EDOM, ex.ErrorCode());
            Assert::AreEqual(std::wstring(L"context"), ex.ActionContext());
            Assert::AreEqual(std::wstring(L"Domain error"), ex.Message());
        }

        TEST_METHOD(CreateWindowsApiException)
        {
            // ERROR_SUCCESS
            WindowsApiException ex(0ul, L"context");

            Assert::AreEqual(0ul, ex.ErrorCode());
            Assert::AreEqual(std::wstring(L"context"), ex.ActionContext());
            Assert::AreEqual(std::wstring(L"The operation completed successfully.\r\n"), ex.Message());
        }

    };
}