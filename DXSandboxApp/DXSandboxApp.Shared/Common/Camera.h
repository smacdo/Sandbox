#pragma once

namespace DXSandboxApp
{
    // Camera defines the position, orientation and viewing frustum of a virtual camera looking into a 3d world.
    class Camera
    {
    public:
        Camera();
        virtual ~Camera();

        void SetViewParams(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 lookAt, DirectX::XMFLOAT3 up);
        void SetProjParams(float fieldOfView, float aspectRatio, float nearPlane, float farPlane);

        void SetLookDirection(DirectX::XMFLOAT3 lookDirection);
        void SetPosition(DirectX::XMFLOAT3 position);

        DirectX::XMMATRIX View() const;
        DirectX::XMMATRIX Projection() const;
        DirectX::XMMATRIX World() const;
        DirectX::XMFLOAT3 Position() const;
        DirectX::XMFLOAT3 LookAt() const;
        DirectX::XMFLOAT3 Up() const;
        float NearClipPlane() const;
        float FarClipPlane() const;
        float Pitch() const;
        float Yaw() const;

    protected:
        DirectX::XMFLOAT4X4 mViewMatrix;
        DirectX::XMFLOAT4X4 mProjectionMatrix;
        DirectX::XMFLOAT4X4 mInverseView;

        DirectX::XMFLOAT3 mEye;
        DirectX::XMFLOAT3 mLookAt;
        DirectX::XMFLOAT3 mUp;
        float mCameraYawAngle;
        float mCameraPitchAngle;
        float mFieldOfView;
        float mAspectRatio;
        float mNearPlane;
        float mFarPlane;
    };
}
