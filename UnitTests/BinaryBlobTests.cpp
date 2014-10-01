#include "stdafx.h"
#include "CppUnitTest.h"
#include "DXTestException.h"
#include "BinaryBlob.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{
    // Unit tests for my extensions to SimpleMath.h
    TEST_CLASS(BinaryBlobTests)
    {
    public:
        TEST_METHOD(CreateBinaryBlobDefaultConstructor)
        {
            BinaryBlob blob;

            Assert::AreEqual((char *) nullptr, blob.BufferPointer());
            Assert::AreEqual((size_t)0, (size_t)blob.BufferSize());
        }

        TEST_METHOD(CreateBinaryBlobWithDataPointer)
        {
            const char * SampleData = "Hello World";
            const size_t SampleDataSize = 12;

            BinaryBlob blob(SampleData, SampleDataSize);

            Assert::AreEqual(SampleData, blob.BufferPointer());
            Assert::AreEqual(SampleDataSize, (size_t)blob.BufferSize());
        }

        TEST_METHOD(BinaryBlobPointerConstructorCopiesData)
        {
            const char * SampleData = "Hello World";
            const size_t SampleDataSize = 12;

            BinaryBlob blob(SampleData, SampleDataSize);

            // binary blob data has different memory buffers but the same content.
            Assert::AreNotSame(SampleData, blob.BufferPointer());
            Assert::AreEqual(SampleData, blob.BufferPointer());
        }

        TEST_METHOD(BinaryBlobCopyConstructorCorrectlyCopiesData)
        {
            const char * SampleData = "Hello World";
            const size_t SampleDataSize = 12;

            BinaryBlob firstBlob(SampleData, SampleDataSize);
            BinaryBlob copiedBlob(firstBlob);

            // copied blob has different memory buffer but the same content.
            Assert::AreNotSame(SampleData, copiedBlob.BufferPointer());
            Assert::AreNotSame(firstBlob.BufferPointer(), copiedBlob.BufferPointer());
            Assert::AreEqual(SampleData, copiedBlob.BufferPointer());

            // copied blob should have same size.
            Assert::AreEqual((size_t)firstBlob.BufferSize(), (size_t)copiedBlob.BufferSize());
        }

        TEST_METHOD(BinaryBlobCopyAssignmentCopiesData)
        {
            const char * SampleData = "Hello World";
            const size_t SampleDataSize = 12;

            BinaryBlob firstBlob(SampleData, SampleDataSize);
            BinaryBlob copiedBlob(new char[8], 8);      // force a non-default constructor to prevent optimization
            
            copiedBlob = firstBlob;

            // copied blob has different memory buffer but the same content.
            Assert::AreNotSame(SampleData, copiedBlob.BufferPointer());
            Assert::AreNotSame(firstBlob.BufferPointer(), copiedBlob.BufferPointer());
            Assert::AreEqual(SampleData, copiedBlob.BufferPointer());

            // copied blob should have same size.
            Assert::AreEqual((size_t)firstBlob.BufferSize(), (size_t)copiedBlob.BufferSize());
        }


        TEST_METHOD(BinaryBlobAssignmentFromNull)
        {
            BinaryBlob firstBlob;
            BinaryBlob copiedBlob(new char[8], 8); // force a non-default constructor to prevent optimization

            copiedBlob = firstBlob;

            Assert::IsNull(copiedBlob.BufferPointer());
            Assert::AreEqual((size_t)0, (size_t)copiedBlob.BufferSize());
        }
    };
}