#include "stdafx.h"
#include "CppUnitTest.h"
#include "Light.h"
#include "SimpleMath.h"
#include "TestHelpers.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace DirectX::SimpleMath;

namespace UnitTests
{
    TEST_CLASS(LightTests)
    {
    public:
        TEST_METHOD(CreateLightAndSetValues)
        {
            Vector4 value4 = Vector4(1.0f, 2.0f, 3.0f, 4.0f);
            Vector3 value3 = Vector3(1.0f, 2.0f, 3.0f);

            Light light;

            // ambient color property
            Assert::AreNotEqual(value4, light.AmbientColor());
            light.SetAmbientColor(value4);
            Assert::AreEqual(value4, light.AmbientColor());

            // diffuse color property
            Assert::AreNotEqual(value4, light.DiffuseColor());
            light.SetDiffuseColor(value4);
            Assert::AreEqual(value4, light.DiffuseColor());

            // specular color property
            Assert::AreNotEqual(value4, light.SpecularColor());
            light.SetSpecularColor(value4);
            Assert::AreEqual(value4, light.SpecularColor());

            // specular power property.
            Assert::AreNotEqual(42.0f, light.SpecularPower());
            light.SetSpecularPower(42.0f);
            Assert::AreEqual(42.0f, light.SpecularPower());

            // direction property.
            Assert::AreNotEqual(value3, light.Direction());
            light.SetDirection(value3);
            Assert::AreEqual(value3, light.Direction());
        }
    };
}