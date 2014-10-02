#pragma once

#include <SimpleMath.h>
#include "size.h"

class Camera
{
public:
	Camera(const Size& screenSize, float screenNear, float screenDepth);

    void SetPosition(const DirectX::SimpleMath::Vector3& position);
	void SetRotation(const DirectX::SimpleMath::Vector3& rotation);

	DirectX::SimpleMath::Vector3 Position() const;
	DirectX::SimpleMath::Vector3 Rotation() const;
	DirectX::SimpleMath::Matrix ViewMatrix() const;
    DirectX::SimpleMath::Matrix ProjectionMatrix() const { return mProjectionMatrix; }
    DirectX::SimpleMath::Matrix OrthoMatrix() const { return mOrthoMatrix; }

    bool IsViewMatrixDirty() const;

	void Render() const;
    float FieldOfView() const { return mFieldOfView; }
    float AspectRatio() const { return mAspectRatio; }

protected:
    void RegenerateProjectionMatrix();
    void RegenerateOrthoMatrix();

private:
    bool mRegenerateViewMatrix;
    float mScreenWidth;
    float mScreenHeight;
    float mFieldOfView;
    float mAspectRatio;
    float mScreenNear;
    float mScreenDepth;
    DirectX::SimpleMath::Vector3 mPosition;
    DirectX::SimpleMath::Vector3 mRotation;
	mutable DirectX::SimpleMath::Matrix mViewMatrix;
    DirectX::SimpleMath::Matrix mProjectionMatrix;
    DirectX::SimpleMath::Matrix mOrthoMatrix;
};