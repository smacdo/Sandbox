#include "pch.h"
#include "ConfigurableDesc.h"
#include "Common/DirectXHelper.h"

using namespace DXSandboxApp;
using namespace Microsoft::WRL;

SamplerSettings::SamplerSettings()
{
    ZeroMemory(&mDesc, sizeof(mDesc));

    mDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    mDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    mDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    mDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    mDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    mDesc.MinLOD = 0;
    mDesc.MaxLOD = FLT_MAX;
}

ID3D11SamplerState * SamplerSettings::CreateSamplerState(ID3D11Device * pDevice) const
{
    ComPtr<ID3D11SamplerState> sampler;
    DX::ThrowIfFailed(pDevice->CreateSamplerState(&mDesc, &sampler));

    return sampler.Detach();
}

SamplerSettings& SamplerSettings::Default()
{
    return LinearFilter().ClampU().ClampV().MinLod(0).MaxLod(FLT_MAX).MaxAnisotropy(1).CompareNever();
}

SamplerSettings& SamplerSettings::PointFilter()
{
    mDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    return *this;
}

SamplerSettings& SamplerSettings::LinearFilter()
{
    mDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    return *this;
}

SamplerSettings& SamplerSettings::AnisotropicFilter()
{
    mDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    return *this;
}

SamplerSettings& SamplerSettings::WrapU()
{
    mDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    return *this;
}

SamplerSettings& SamplerSettings::MirrorU()
{
    mDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
    return *this;
}

SamplerSettings& SamplerSettings::MirrorOnceU()
{
    mDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
    return *this;
}

SamplerSettings& SamplerSettings::ClampU()
{
    mDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    return *this;
}

SamplerSettings& SamplerSettings::BorderU()
{
    mDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    return *this;
}

SamplerSettings& SamplerSettings::WrapV()
{
    mDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    return *this;
}

SamplerSettings& SamplerSettings::MirrorV()
{
    mDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
    return *this;
}

SamplerSettings& SamplerSettings::MirrorOnceV()
{
    mDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
    return *this;
}

SamplerSettings& SamplerSettings::ClampV()
{
    mDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    return *this;
}

SamplerSettings& SamplerSettings::BorderV()
{
    mDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    return *this;
}

SamplerSettings& SamplerSettings::MaxAnisotropy(unsigned int max)
{
    mDesc.MaxAnisotropy = max;
    return *this;
}

SamplerSettings& SamplerSettings::CompareNever()
{
    mDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    return *this;
}

SamplerSettings& SamplerSettings::CompareLess()
{
    mDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
    return *this;
}

SamplerSettings& SamplerSettings::CompareEqual()
{
    mDesc.ComparisonFunc = D3D11_COMPARISON_EQUAL;
    return *this;
}

SamplerSettings& SamplerSettings::CompareLessEqual()
{
    mDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
    return *this;
}

SamplerSettings& SamplerSettings::CompareGreater()
{
    mDesc.ComparisonFunc = D3D11_COMPARISON_GREATER;
    return *this;
}

SamplerSettings& SamplerSettings::CompareNotEqual()
{
    mDesc.ComparisonFunc = D3D11_COMPARISON_NOT_EQUAL;
    return *this;
}

SamplerSettings& SamplerSettings::CompareGreaterEqual()
{
    mDesc.ComparisonFunc = D3D11_COMPARISON_GREATER_EQUAL;
    return *this;
}

SamplerSettings& SamplerSettings::CompareAlways()
{
    mDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    return *this;
}

SamplerSettings& SamplerSettings::MinLod(float min)
{
    mDesc.MinLOD = min;
    return *this;
}

SamplerSettings& SamplerSettings::MaxLod(float max)
{
    mDesc.MaxLOD = max;
    return *this;
}