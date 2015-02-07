#include "pch.h"
#include "Camera.h"

using namespace DXSandboxApp;
using namespace DirectX;

#undef min
#undef max

/**
* \brief
*/
Camera::Camera()
{
    // Setup the view matrix.
    SetViewParams(
        XMFLOAT3(0.0f, 0.0f, 0.0f),   // Default eye position.
        XMFLOAT3(0.0f, 0.0f, 1.0f),   // Default look at position.
        XMFLOAT3(0.0f, 1.0f, 0.0f));  // Default up vector.

    // Setup the projection matrix.
    SetProjParams(XM_PI / 4, 1.0f, 1.0f, 1000.0f);
}

/**
 * \brief
 */
Camera::~Camera()
{

}

/**
 * \brief
 */
void Camera::SetLookDirection(XMFLOAT3 lookDirection)
{
    XMFLOAT3 lookAt;

    lookAt.x = mEye.x + lookDirection.x;
    lookAt.y = mEye.y + lookDirection.y;
    lookAt.z = mEye.z + lookDirection.z;

    SetViewParams(mEye, lookAt, mUp);
}

void Camera::SetPosition(XMFLOAT3 position)
{
    SetViewParams(position, mLookAt, mUp);
}

void Camera::SetViewParams(
    XMFLOAT3 eye,
    XMFLOAT3 lookAt,
    XMFLOAT3 up)
{
    mEye = eye;
    mLookAt = lookAt;
    mUp = up;

    // Calculate the view matrix.
    XMMATRIX view = XMMatrixLookAtLH(
        XMLoadFloat3(&mEye),
        XMLoadFloat3(&mLookAt),
        XMLoadFloat3(&mUp));

    XMVECTOR det;
    XMMATRIX inverseView = XMMatrixInverse(&det, view);

    XMStoreFloat4x4(&mViewMatrix, view);
    XMStoreFloat4x4(&mInverseView, inverseView);

    // The axis basis vectors and camera position are stored inside the position matrix in the 4 rows of the camera's
    // world matrix. To figure out the yaw/pitch of the camera, we just need the Z basis vector.
    XMFLOAT3 zBasis;
    XMStoreFloat3(&zBasis, inverseView.r[2]);

    mCameraYawAngle = atan2f(zBasis.x, zBasis.z);

    float len = sqrtf(zBasis.z * zBasis.z + zBasis.x * zBasis.x);
    mCameraPitchAngle = atan2f(zBasis.y, len);
}

void Camera::SetProjParams(
    float fieldOfView,
    float aspectRatio,
    float nearPlane,
    float farPlane)
{
    // Set attributes for the projection matrix.
    mFieldOfView = fieldOfView;
    mAspectRatio = aspectRatio;
    mNearPlane = nearPlane;
    mFarPlane = farPlane;

    XMStoreFloat4x4(
        &mProjectionMatrix,
        XMMatrixPerspectiveFovLH(
        mFieldOfView,
        mAspectRatio,
        mNearPlane,
        mFarPlane));
}

DirectX::XMMATRIX Camera::View() const
{
    return XMLoadFloat4x4(&mViewMatrix);
}

DirectX::XMMATRIX Camera::Projection() const
{
    return XMLoadFloat4x4(&mProjectionMatrix);
}

DirectX::XMMATRIX Camera::World() const
{
    return XMLoadFloat4x4(&mInverseView);
}

DirectX::XMFLOAT3 Camera::Position() const
{
    return mEye;
}

DirectX::XMFLOAT3 Camera::LookAt() const
{
    return mLookAt;
}

float Camera::NearClipPlane() const
{
    return mNearPlane;
}

float Camera::FarClipPlane() const
{
    return mFarPlane;
}

float Camera::Pitch() const
{
    return mCameraPitchAngle;
}

float Camera::Yaw() const
{
    return mCameraYawAngle;
}