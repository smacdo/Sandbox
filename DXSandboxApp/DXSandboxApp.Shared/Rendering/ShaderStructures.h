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
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT2 texture;
        DirectX::XMFLOAT3 normal;
    };
}