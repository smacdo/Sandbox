#include "stdafx.h"
#include "CppUnitTest.h"
#include "Utils.h"

#include <string>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{
    // Unit tests for my extensions to SimpleMath.h
    TEST_CLASS(UtilTests)
    {
    public:
        TEST_METHOD(ConvertUtf8ToWString)
        {
            // Convert a string to wstring
            std::string initial = "Hello World";
            std::wstring expected = L"Hello World";
            std::wstring actual = Utils::ConvertUtf8ToWString(initial);

            Assert::AreEqual(expected, actual);
        }

        TEST_METHOD(ConvertUtf16ToUtf8)
        {
            // Convert a string to wstring
            std::wstring initial = L"Hello World";
            std::string expected = "Hello World";
            std::string actual = Utils::ConvertUtf16ToUtf8(initial);

            Assert::AreEqual(expected, actual);
        }

        TEST_METHOD(GetErrorMessageFromWinApiErrorCode)
        {
            // ERROR_SUCCESS
            Assert::AreEqual(
                std::wstring(L"The operation completed successfully.\r\n"),
                Utils::GetErrorMessageFromWinApiErrorCode(0u));

            // ERROR_SUCCESS
            Assert::AreEqual(
                std::wstring(L"Incorrect function.\r\n"),
                Utils::GetErrorMessageFromWinApiErrorCode(1u));
        }

        TEST_METHOD(EndsWith)
        {
            // Convert a string to wstring
            std::string initial = "Hello World";

            Assert::IsTrue(Utils::EndsWith(initial, "d"));
            Assert::IsTrue(Utils::EndsWith(initial, "World"));
            Assert::IsTrue(Utils::EndsWith(initial, "Hello World"));
            Assert::IsFalse(Utils::EndsWith(initial, "world"));
            Assert::IsFalse(Utils::EndsWith(initial, "x"));
        }

    };
}