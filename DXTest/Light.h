#pragma once

#include "SimpleMath.h"

// Stores lighting information.
class Light
{
public:
    // TODO: Add params to constructor.
    Light();

    // TODO: Pass these around as vectors. This guy is really inconsistent.
    void SetDiffuseColor(float r, float g, float b, float a);
    void SetAmbientColor(float r, float g, float b, float a);
    void SetDirection(float x, float y, float z);
    void SetSpecularColor(float r, float g, float b, float a);
    void SetSpecularPower(float p);

    DirectX::SimpleMath::Vector4 AmbientColor() const;
    DirectX::SimpleMath::Vector4 DiffuseColor() const;
    DirectX::SimpleMath::Vector3 Direction() const;
    DirectX::SimpleMath::Vector4 SpecularColor() const;
    float SpecularPower() const;

private:
    DirectX::SimpleMath::Vector4 mAmbientColor;
    DirectX::SimpleMath::Vector4 mDiffuseColor;
    DirectX::SimpleMath::Vector4 mSpecularColor;
    DirectX::SimpleMath::Vector3 mDirection;
    float mSpecularPower;
};

