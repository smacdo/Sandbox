#include "Camera.h"

using namespace DirectX::SimpleMath;

Camera::Camera()
: mX(0.0f),
  mY(0.0f),
  mZ(0.0f),
  mRX(0.0f),
  mRY(0.0f),
  mRZ(0.0f),
  mViewMatrix()
{
}

Camera::Camera(const Camera& camera)
: mX(camera.mX),
  mY(camera.mY),
  mZ(camera.mZ),
  mRX(camera.mRX),
  mRY(camera.mRY),
  mRZ(camera.mRZ),
  mViewMatrix(camera.mViewMatrix)
{
}


Camera::~Camera()
{
}

Camera& Camera::operator =(const Camera& camera)
{
	mX = camera.mX;
	mY = camera.mY;
	mZ = camera.mZ;
	mRX = camera.mRX;
	mRY = camera.mRY;
	mRZ = camera.mRZ;
	mViewMatrix = camera.mViewMatrix;

	return *this;
}

void Camera::SetPosition(float x, float y, float z)
{
	mX = x;
	mY = y;
	mZ = z;
}

void Camera::SetRotation(float rX, float rY, float rZ)
{
	mRX = rX;
	mRY = rY;
	mRZ = rZ;
}

Vector3 Camera::Position() const
{
	return DirectX::SimpleMath::Vector3(mX, mY, mZ);
}

Vector3 Camera::Rotation() const
{
	return DirectX::SimpleMath::Vector3(mRX, mRY, mRZ);
}

Matrix Camera::ViewMatrix() const
{
	return mViewMatrix;
}

// TODO: This name is TERRIBLE.
//  -> Camera::RegenerateValues or something.
void Camera::Render()
{
	// Initialize camera values.
	Vector3 up = Vector3(0.0f, 1.0f, 0.0f);		// up is +y
	Vector3 position = Vector3(mX, mY, mZ);
	Vector3 lookAt = Vector3(0.0f, 0.0f, 1.0f);	// look down +z axis

	// Convert yaw (y axis), pitch (x axis) and roll (a axis) to radian value.
	float pitch = mRX * 0.0174532925f;
	float yaw = mRY * 0.0174532925f;
	float roll = mRZ * 0.0174532925f;

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