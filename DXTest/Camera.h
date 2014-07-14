#pragma once

#include <SimpleMath.h>

class Camera
{
public:
	Camera();
	Camera(const Camera& camera);
	virtual ~Camera();

	Camera& operator =(const Camera& camera);

	void SetPosition(float x, float y, float z);
	void SetRotation(float rX, float rY, float rZ);

	DirectX::SimpleMath::Vector3 Position() const;
	DirectX::SimpleMath::Vector3 Rotation() const;
	DirectX::SimpleMath::Matrix ViewMatrix() const;

	void Render();
private:
	float mX, mY, mZ;
	float mRX, mRY, mRZ;
	DirectX::SimpleMath::Matrix mViewMatrix;
};

