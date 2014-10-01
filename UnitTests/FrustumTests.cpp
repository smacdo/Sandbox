#include "stdafx.h"
#include "CppUnitTest.h"
#include "Frustum.h"
#include "SimpleMath.h"
#include "TestHelpers.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace DirectX::SimpleMath;

namespace UnitTests
{
    TEST_CLASS(FrustumTests)
    {
    public:
        TEST_METHOD(CreateFrustum)
        {
            Frustum f;
        }
    };
}