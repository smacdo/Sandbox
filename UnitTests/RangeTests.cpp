#include "stdafx.h"
#include "CppUnitTest.h"
#include "Range.h"

#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{
    TEST_CLASS(RangeTests)
    {
    public:
        TEST_METHOD(ConstantRangeSummation)
        {
            std::vector<int> values = { 1, 2, 3 };
            int sum = 0;

            for (auto i : MakeRange<0, 3>())
            {
                sum += values[i];
            }

            Assert::AreEqual(6, sum);
        }

        TEST_METHOD(RangeSummation)
        {
            std::vector<int> values = { 1, 2, 3 };
            int sum = 0;

            for (auto i : MakeRange(0, 3))
            {
                sum += values[i];
            }

            Assert::AreEqual(6, sum);
        }

    };
}