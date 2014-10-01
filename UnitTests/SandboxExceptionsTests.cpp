#include "stdafx.h"
#include "CppUnitTest.h"
#include "DXTestException.h"
#include "Utils.h"

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

        TEST_METHOD(CreateDirectXTestException)
        {
            // DXGI_ERROR_INVALID_CALL
            DirectXException ex(0x887a0001, L"context");

            Assert::AreEqual((unsigned long)0x887a0001, ex.ErrorCode());
            Assert::AreEqual(std::wstring(L"context"), ex.ActionContext());
            Assert::IsTrue(Utils::StartsWith(ex.Message(), L"The application made a call that is invalid"));
        }

        TEST_METHOD(CreateDirectXTestException_2)
        {
            // DXGI_ERROR_INVALID_CALL
            DirectXException ex(0x887a0001, L"context", L"filename", 100);

            Assert::AreEqual((unsigned long)0x887a0001, ex.ErrorCode());
            Assert::AreEqual(std::wstring(L"context"), ex.ActionContext());
            Assert::AreEqual(std::wstring(L"filename"), ex.FileName());
            Assert::AreEqual(100, ex.LineNumber());
            Assert::IsTrue(Utils::StartsWith(ex.Message(), L"The application made a call that is invalid"));
        }

    };
}