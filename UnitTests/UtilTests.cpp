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
    private:
        void WriteToBuffer(char * pOutputBuffer, size_t length)
        {
            strcpy_s(pOutputBuffer, length, "Hello World");
        }

        void WriteToBuffer(wchar_t * pOutputBuffer, size_t length)
        {
            wcscpy_s(pOutputBuffer, length, L"Hello World");
        }

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

        TEST_METHOD(GetErrorMessageFromErrno)
        {
            // ERANGE
            Assert::AreEqual(
                std::wstring(L"Domain error"),
                Utils::GetErrorMessageFromErrno(EDOM));
        }

        TEST_METHOD(StartsWith)
        {
            std::string initial = "Hello World";

            Assert::IsTrue(Utils::StartsWith(initial, "H"));
            Assert::IsTrue(Utils::StartsWith(initial, "Hello "));
            Assert::IsTrue(Utils::StartsWith(initial, "Hello World"));
            Assert::IsFalse(Utils::StartsWith(initial, "h"));
            Assert::IsFalse(Utils::StartsWith(initial, "Hx"));
            Assert::IsFalse(Utils::StartsWith(initial, "x"));
            Assert::IsFalse(Utils::StartsWith(initial, ""));
            Assert::IsFalse(Utils::StartsWith(initial, "Hello World!"));
        }

        TEST_METHOD(StartsWith_Wide)
        {
            std::wstring initial = L"Hello World";

            Assert::IsTrue(Utils::StartsWith(initial, L"H"));
            Assert::IsTrue(Utils::StartsWith(initial, L"Hello "));
            Assert::IsTrue(Utils::StartsWith(initial, L"Hello World"));
            Assert::IsFalse(Utils::StartsWith(initial, L"h"));
            Assert::IsFalse(Utils::StartsWith(initial, L"Hx"));
            Assert::IsFalse(Utils::StartsWith(initial, L"x"));
            Assert::IsFalse(Utils::StartsWith(initial, L""));
            Assert::IsFalse(Utils::StartsWith(initial, L"Hello World!"));
        }

        TEST_METHOD(EndsWith)
        {
            std::string initial = "Hello World";

            Assert::IsTrue(Utils::EndsWith(initial, "d"));
            Assert::IsTrue(Utils::EndsWith(initial, "World"));
            Assert::IsTrue(Utils::EndsWith(initial, "Hello World"));
            Assert::IsFalse(Utils::EndsWith(initial, "world"));
            Assert::IsFalse(Utils::EndsWith(initial, "x"));
            Assert::IsFalse(Utils::EndsWith(initial, ""));
            Assert::IsFalse(Utils::EndsWith(initial, "Hello World!"));
        }

        TEST_METHOD(EndsWith_Wide)
        {
            std::wstring initial = L"Hello World";

            Assert::IsTrue(Utils::EndsWith(initial, L"d"));
            Assert::IsTrue(Utils::EndsWith(initial, L"World"));
            Assert::IsTrue(Utils::EndsWith(initial, L"Hello World"));
            Assert::IsFalse(Utils::EndsWith(initial, L"world"));
            Assert::IsFalse(Utils::EndsWith(initial, L"x"));
            Assert::IsFalse(Utils::EndsWith(initial, L""));
            Assert::IsFalse(Utils::EndsWith(initial, L"Hello World!"));
        }

        TEST_METHOD(TrimLeft)
        {
            Assert::AreEqual(std::string("hello"), Utils::LeftTrim("hello"));
            Assert::AreEqual(std::string("hello"), Utils::LeftTrim(" \t hello"));
            Assert::AreEqual(std::string("hello\t  "), Utils::LeftTrim(" \thello\t  "));
        }

        TEST_METHOD(TrimLeft_Wide)
        {
            Assert::AreEqual(std::wstring(L"hello"), Utils::LeftTrim(L"hello"));
            Assert::AreEqual(std::wstring(L"hello"), Utils::LeftTrim(L" \t hello"));
            Assert::AreEqual(std::wstring(L"hello\t  "), Utils::LeftTrim(L" \thello\t  "));
        }

        TEST_METHOD(TrimRight)
        {
            Assert::AreEqual(std::string("hello"), Utils::RightTrim("hello"));
            Assert::AreEqual(std::string("hello"), Utils::RightTrim("hello \t "));
            Assert::AreEqual(std::string("  \thello"), Utils::RightTrim("  \thello\t "));
        }

        TEST_METHOD(TrimRight_Wide)
        {
            Assert::AreEqual(std::wstring(L"hello"), Utils::RightTrim(L"hello"));
            Assert::AreEqual(std::wstring(L"hello"), Utils::RightTrim(L"hello \t "));
            Assert::AreEqual(std::wstring(L"  \thello"), Utils::RightTrim(L"  \thello\t "));
        }

        TEST_METHOD(Trim)
        {
            Assert::AreEqual(std::string("hello"), Utils::Trim("hello"));
            Assert::AreEqual(std::string("hello"), Utils::Trim(" \t hello"));
            Assert::AreEqual(std::string("hello"), Utils::Trim(" \thello\t  "));

            Assert::AreEqual(std::string("hello"), Utils::Trim("hello"));
            Assert::AreEqual(std::string("hello"), Utils::Trim("hello \t "));
            Assert::AreEqual(std::string("hello"), Utils::Trim("  \thello\t "));
        }

        TEST_METHOD(Trim_Wide)
        {
            Assert::AreEqual(std::wstring(L"hello"), Utils::Trim(L"hello"));
            Assert::AreEqual(std::wstring(L"hello"), Utils::Trim(L" \t hello"));
            Assert::AreEqual(std::wstring(L"hello"), Utils::Trim(L" \thello\t  "));

            Assert::AreEqual(std::wstring(L"hello"), Utils::Trim(L"hello"));
            Assert::AreEqual(std::wstring(L"hello"), Utils::Trim(L"hello \t "));
            Assert::AreEqual(std::wstring(L"hello"), Utils::Trim(L"  \thello\t "));
        }

        TEST_METHOD(MakeString)
        {
            Assert::AreEqual(
                std::string("Hello World"),
                (Utils::MakeString() << "Hello " << "World").operator std::string());

            Assert::AreEqual(
                std::string("Answer 42!"),
                (Utils::MakeString() << "Answer " << 42 << "!").operator std::string());
        }

        TEST_METHOD(MakeString_Wide)
        {
            Assert::AreEqual(
                std::wstring(L"Hello World"),
                (Utils::MakeWideString() << "Hello " << "World").operator std::wstring());

            Assert::AreEqual(
                std::wstring(L"Hello World"),
                (Utils::MakeWideString() << L"Hello " << L"World").operator std::wstring());

            Assert::AreEqual(
                std::wstring(L"Answer 42!"),
                (Utils::MakeWideString() << L"Answer " << 42 << "!").operator std::wstring());
        }

        TEST_METHOD(WinStringBuffer)
        {
            const size_t MaxSize = 32;
            std::string actual;

            WriteToBuffer(Utils::WinStringBuffer(actual, MaxSize), MaxSize);

            Assert::AreEqual(std::string("Hello World"), actual);
        }

        TEST_METHOD(WinStringBufferWide)
        {
            const size_t MaxSize = 32;
            std::wstring actual;

            WriteToBuffer(Utils::WinStringBuffer(actual, MaxSize), MaxSize);

            Assert::AreEqual(std::wstring(L"Hello World"), actual);
        }
    };
}