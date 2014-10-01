#include "stdafx.h"
#include "Light.h"
#include "SimpleMath.h"

using namespace DirectX::SimpleMath;

Light::Light()
: mAmbientColor(),
  mDiffuseColor(),
  mSpecularColor(),
  mSpecularPower(),
  mDirection()
{
}

void Light::SetAmbientColor(const DirectX::SimpleMath::Vector4& color)
{
    mAmbientColor = color;
}

void Light::SetDiffuseColor(const DirectX::SimpleMath::Vector4& color)
{
    mDiffuseColor = color;
}

void Light::SetDirection(const DirectX::SimpleMath::Vector3& direction)
{
    mDirection = direction;
}

void Light::SetSpecularColor(const DirectX::SimpleMath::Vector4& color)
{
    mSpecularColor = color;
}

void Light::SetSpecularPower(float p)
{
    mSpecularPower = p;
}

Vector4 Light::AmbientColor() const
{
    return mAmbientColor;
}

Vector4 Light::DiffuseColor() const
{
    return mDiffuseColor;
}

Vector3 Light::Direction() const
{
    return mDirection;
}

Vector4 Light::SpecularColor() const
{
    return mSpecularColor;
}

float Light::SpecularPower() const
{
    return mSpecularPower;
}