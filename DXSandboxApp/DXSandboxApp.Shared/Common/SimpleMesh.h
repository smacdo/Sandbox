#pragma once

#include <vector>

struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct D3D11_INPUT_ELEMENT_DESC;

namespace DXSandboxApp
{
    // in-memory software mesh format.
    struct SimpleMeshVertex
    {
        float x, y, z;
        float tu, tv;
        float nx, ny, nz;
    };

    struct SimpleMeshData
    {
        SimpleMeshData();

        std::vector<SimpleMeshVertex> vertices;
        std::vector<int> indices;
    };
}
