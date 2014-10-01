#pragma once

#include <SimpleMath.h>

class Camera
{
public:
	Camera();

    void SetPosition(const DirectX::SimpleMath::Vector3& position);
	void SetRotation(const DirectX::SimpleMath::Vector3& rotation);

	DirectX::SimpleMath::Vector3 Position() const;
	DirectX::SimpleMath::Vector3 Rotation() const;
	DirectX::SimpleMath::Matrix ViewMatrix() const;

    bool IsViewMatrixDirty() const;

	void Render() const;

private:
    bool mRegenerateViewMatrix;
    DirectX::SimpleMath::Vector3 mPosition;
    DirectX::SimpleMath::Vector3 mRotation;
	mutable DirectX::SimpleMath::Matrix mViewMatrix;
};