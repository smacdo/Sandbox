#include "stdafx.h"
#include "CppUnitTest.h"
#include "Camera.h"
#include "SimpleMath.h"
#include "TestHelpers.h"
#include "Size.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace DirectX::SimpleMath;

namespace UnitTests
{
    TEST_CLASS(CameraTests)
    {
    private:
        const Size DefaultScreenSize = { 800, 600 };
        const float DefaultNear = 0.1f;
        const float DefaultDepth = 1000.0f;

    public:
        TEST_METHOD(CreateCameraAndSetValues)
        {
            Vector3 zero3(0.0f, 0.f, 0.0f);
            Vector3 v3(1.0f, 2.0f, 3.0f);

            Camera camera(DefaultScreenSize, DefaultNear, DefaultDepth);

            Assert::AreEqual(zero3, camera.Position());
            camera.SetPosition(v3);
            Assert::AreEqual(v3, camera.Position());

            Assert::AreEqual(zero3, camera.Rotation());
            camera.SetRotation(v3);
            Assert::AreEqual(v3, camera.Rotation());
        }

        TEST_METHOD(SetCameraPositionOrRotationRegeneratesViewMatrix)
        {
            Vector3 v3(1.0f, 2.0f, 3.0f);

            // Test set position sets dirty flag.
            {
                Camera camera(DefaultScreenSize, DefaultNear, DefaultDepth);
                Assert::IsFalse(camera.IsViewMatrixDirty());

                camera.SetPosition(v3);
                Assert::IsTrue(camera.IsViewMatrixDirty());
            }
            
            // Test set rotation sets dirty flag.
            {
                Camera camera(DefaultScreenSize, DefaultNear, DefaultDepth);
                Assert::IsFalse(camera.IsViewMatrixDirty());

                camera.SetRotation(v3);
                Assert::IsTrue(camera.IsViewMatrixDirty());
            }
        }

        TEST_METHOD(SetCameraValuesSetsViewMatrixCorrectly)
        {
            Vector3 position(1.0f, 2.0f, 3.0f);
            Vector3 rotation(0.2f, 0.4f, 0.6f);

            Camera camera(DefaultScreenSize, DefaultNear, DefaultDepth);

            camera.SetPosition(position);
            camera.SetRotation(rotation);

            Matrix viewMatrix = camera.ViewMatrix();

            // TODO: Test within tolerance.
            //Assert::AreEqual(viewMatrix.Translation(), -position);

            // TODO: Test rotation.
        }
    };
}