#pragma once

struct ID3D11SamplerState;
struct ID3D11Device;

namespace DXSandboxApp
{
    class SamplerSettings
    {
    public:
        SamplerSettings();

        ID3D11SamplerState * CreateSamplerState(ID3D11Device * device) const;

        SamplerSettings& Default();
        SamplerSettings& PointFilter();
        SamplerSettings& LinearFilter();
        SamplerSettings& AnisotropicFilter();
        SamplerSettings& WrapU();
        SamplerSettings& MirrorU();
        SamplerSettings& MirrorOnceU();
        SamplerSettings& ClampU();
        SamplerSettings& BorderU();
        SamplerSettings& WrapV();
        SamplerSettings& MirrorV();
        SamplerSettings& MirrorOnceV();
        SamplerSettings& ClampV();
        SamplerSettings& BorderV();
        SamplerSettings& MaxAnisotropy(unsigned int max);
        SamplerSettings& CompareNever();
        SamplerSettings& CompareLess();
        SamplerSettings& CompareEqual();
        SamplerSettings& CompareLessEqual();
        SamplerSettings& CompareGreater();
        SamplerSettings& CompareNotEqual();
        SamplerSettings& CompareGreaterEqual();
        SamplerSettings& CompareAlways();
        SamplerSettings& MinLod(float min);
        SamplerSettings& MaxLod(float max);

    private:
        D3D11_SAMPLER_DESC mDesc;
    };
}

