#pragma once

#include "SimpleMath.h"

/**
 * \brief Holds values required for lighting computation.
 */
class Light
{
public:
    Light();

    void SetDirection(const DirectX::SimpleMath::Vector3& direction);

    void SetAmbientColor(const DirectX::SimpleMath::Vector4& color);
    void SetDiffuseColor(const DirectX::SimpleMath::Vector4& color);
    void SetSpecularColor(const DirectX::SimpleMath::Vector4& color);
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

