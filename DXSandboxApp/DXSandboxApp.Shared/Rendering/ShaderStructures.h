#pragma once

namespace DXSandboxApp
{
	// Used to send per-vertex data to the vertex shader.
	struct VertexPositionColor
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 color;
	};

    struct VertexSimple
    {
        VertexSimple(float px, float py, float pz, float tu, float tv, float nx, float ny, float nz)
            : pos(px, py, pz),
              texture(tu, tv),
              normal(nx, ny, nz)
        {
        }

        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT2 texture;
        DirectX::XMFLOAT3 normal;
    };
}