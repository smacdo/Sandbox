#include "stdafx.h"
#include "Camera.h"

using namespace DirectX::SimpleMath;

Camera::Camera(const Size& screenSize, float screenNear, float screenDepth)
    : mRegenerateViewMatrix(false),
      mScreenWidth(static_cast<float>(screenSize.width)),
      mScreenHeight(static_cast<float>(screenSize.height)),
      mFieldOfView(3.141592653589793f / 4.0f),
      mAspectRatio(mScreenWidth / mScreenHeight),
      mScreenNear(screenNear),
      mScreenDepth(screenDepth),
      mPosition(0.0f, 0.0f, 0.0f),
      mRotation(0.0f, 0.0f, 0.0f),
      mViewMatrix(),
      mProjectionMatrix()
{
    RegenerateProjectionMatrix();
    RegenerateOrthoMatrix();
}

void Camera::SetPosition(const Vector3& position)
{
    mPosition = position;
    mRegenerateViewMatrix = true;
}

void Camera::SetRotation(const Vector3& rotation)
{
    mRotation = rotation;
    mRegenerateViewMatrix = true;
}

Vector3 Camera::Position() const
{
    return mPosition;
}

Vector3 Camera::Rotation() const
{
    return mRotation;
}

Matrix Camera::ViewMatrix() const
{
    if (IsViewMatrixDirty())
    {
        Render();
    }

	return mViewMatrix;
}

bool Camera::IsViewMatrixDirty() const
{
    return mRegenerateViewMatrix;
}

// TODO: This name is TERRIBLE.
//  -> Camera::RegenerateValues or something.
void Camera::Render() const
{
	// Initialize camera values.
	Vector3 up = Vector3(0.0f, 1.0f, 0.0f);		// up is +y
    Vector3 position = mPosition;
	Vector3 lookAt = Vector3(0.0f, 0.0f, 1.0f);	// look down +z axis

	// Convert yaw (y axis), pitch (x axis) and roll (a axis) to radian value.
	float pitch = mRotation.x * 0.0174532925f;
	float yaw = mRotation.y * 0.0174532925f;
	float roll = mRotation.z * 0.0174532925f;

	// Generate rotation amtrix from the camera' yaw, pitch and roll.
	Matrix rotationMatrix = Matrix::CreateFromYawPitchRoll(yaw, pitch, roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	lookAt = Vector3::Transform(lookAt, rotationMatrix);
    up = Vector3::Transform(up, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAt = position + lookAt;

	// Generate the view matrix from the three updated vectors.
    mViewMatrix = DirectX::XMMatrixLookAtLH(position, lookAt, up);
}

void Camera::RegenerateProjectionMatrix()
{
    // Create the projection matrix. The projection matrix will be used to translate the 3d scene into a 2d viewport
    // space that was created above. We need to keep a copy of this matrix so we can pass it to our shaders.
    mProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(mFieldOfView, mAspectRatio, mScreenNear, mScreenDepth);
}

void Camera::RegenerateOrthoMatrix()
{
    // Create an orthographic projection matrix for 2d rendering. This matrix will be used to render 2d elements like the
    // user inteface.
    mOrthoMatrix = DirectX::XMMatrixOrthographicLH(mScreenWidth, mScreenHeight, mScreenNear, mScreenDepth);
}