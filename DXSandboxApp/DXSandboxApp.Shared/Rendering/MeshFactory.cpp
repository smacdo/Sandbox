#include "pch.h"
#include "MeshFactory.h"
#include "Common/DirectXHelper.h"

#include <vector>
#include "Rendering\ShaderStructures.h"

using namespace DXSandboxApp;


void MeshFactory::CreateCubeMesh(
    _In_ ID3D11Device * pDevice,           // TODO: Downgrade to ID3D11Device.
    _Out_ ID3D11Buffer ** vertexBufferOut,
    _Out_ ID3D11Buffer ** indexBufferOut,
    _Out_opt_ unsigned int * vertexCountOut,
    _Out_opt_ unsigned int * indexCountOut)
{
    std::vector<VertexSimple> vertices = CreateCubeVertices();
    std::vector<unsigned short> indices = CreateCubeIndices();

    if (vertexCountOut != nullptr)
    {
        *vertexCountOut = vertices.size();
    }

    // Initialize vertex buffer description and data structures, and then ask DirectX to create a new vertex buffer.
    CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(VertexSimple) * vertices.size(), D3D11_BIND_VERTEX_BUFFER);

    D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
    vertexBufferData.pSysMem = &vertices[0];
    vertexBufferData.SysMemPitch = 0;
    vertexBufferData.SysMemSlicePitch = 0;

    DX::ThrowIfFailed(
        pDevice->CreateBuffer(
            &vertexBufferDesc,
            &vertexBufferData,
            vertexBufferOut));

    if (indexCountOut != nullptr)
    {
        *indexCountOut = indices.size();
    }

    // Initialize index buffer description and data structures, and then ask DirectX to create a new index buffer.
    CD3D11_BUFFER_DESC indexBufferDesc(sizeof(unsigned short) * indices.size(), D3D11_BIND_INDEX_BUFFER);

    D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
    indexBufferData.pSysMem = &indices[0];
    indexBufferData.SysMemPitch = 0;
    indexBufferData.SysMemSlicePitch = 0;

    DX::ThrowIfFailed(
        pDevice->CreateBuffer(
            &indexBufferDesc,
            &indexBufferData,
            indexBufferOut));
}

// Load mesh indices. Each trio of indices represents a triangle to be rendered on the screen.
// For example: 0,2,1 means that the vertices with indexes 0, 2 and 1 from the vertex buffer compose the first
//    
std::vector<unsigned short> MeshFactory::CreateCubeIndices()
{
    return std::vector<unsigned short>(
    {
        // Front Face
        0, 1, 2,
        0, 2, 3,

        // Back Face
        4, 5, 6,
        4, 6, 7,

        // Top Face
        8, 9, 10,
        8, 10, 11,

        // Bottom Face
        12, 13, 14,
        12, 14, 15,

        // Left Face
        16, 17, 18,
        16, 18, 19,

        // Right Face
        20, 21, 22,
        20, 22, 23
    });
}

std::vector<VertexSimple> MeshFactory::CreateCubeVertices()
{
    // Load mesh vertices. Each vertex has a position and a color.
    return std::vector<VertexSimple>(
    {
        // Front Face
        VertexSimple(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, -1.0f, -1.0f),
        VertexSimple(-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, -1.0f),
        VertexSimple(1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f),
        VertexSimple(1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f),

        // Back Face
        VertexSimple(-1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f),
        VertexSimple(1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f, 1.0f),
        VertexSimple(1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f),
        VertexSimple(-1.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 1.0f),

        // Top Face
        VertexSimple(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 1.0f, -1.0f),
        VertexSimple(-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f),
        VertexSimple(1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f),
        VertexSimple(1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f),

        // Bottom Face
        VertexSimple(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f),
        VertexSimple(1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, -1.0f, -1.0f),
        VertexSimple(1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f),
        VertexSimple(-1.0f, -1.0f, 1.0f, 1.0f, 0.0f, -1.0f, -1.0f, 1.0f),

        // Left Face
        VertexSimple(-1.0f, -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 1.0f),
        VertexSimple(-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f),
        VertexSimple(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 1.0f, -1.0f),
        VertexSimple(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f),

        // Right Face
        VertexSimple(1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, -1.0f, -1.0f),
        VertexSimple(1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, -1.0f),
        VertexSimple(1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f),
        VertexSimple(1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f),
    });


}