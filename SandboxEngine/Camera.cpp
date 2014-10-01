#include "stdafx.h"
#include "Camera.h"

using namespace DirectX::SimpleMath;

Camera::Camera()
    : mRegenerateViewMatrix(false),
      mPosition(0.0f, 0.0f, 0.0f),
      mRotation(0.0f, 0.0f, 0.0f),
      mViewMatrix()
{
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