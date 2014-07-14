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

void Light::SetAmbientColor(float red, float green, float blue, float alpha)
{
    mAmbientColor = Vector4(red, green, blue, alpha);
}

void Light::SetDiffuseColor(float red, float green, float blue, float alpha)
{
    mDiffuseColor = Vector4(red, green, blue, alpha);
}

void Light::SetDirection(float x, float y, float z)
{
    mDirection = Vector3(x, y, z);
}

void Light::SetSpecularColor(float r, float g, float b, float a)
{
    mSpecularColor = Vector4(r, g, b, a);
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