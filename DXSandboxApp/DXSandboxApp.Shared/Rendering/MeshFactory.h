#pragma once

#include <vector>
#include "Rendering\ShaderStructures.h"

namespace DXSandboxApp
{
    // Generates geometric primtives.
    class MeshFactory
    {
    public:
        MeshFactory() = default;
        ~MeshFactory() = default;

        static void MeshFactory::CreateCubeMesh(
            _In_ ID3D11Device * pDevice,           // TODO: Downgrade to ID3D11Device.
            _Out_ ID3D11Buffer ** vertexBufferOut,
            _Out_ ID3D11Buffer ** indexBufferOut,
            _Out_opt_ unsigned int * vertexCountOut,
            _Out_opt_ unsigned int * indexCountOut);

        static std::vector<unsigned short> CreateCubeIndices();
        static std::vector<VertexSimple> CreateCubeVertices();

    private:
    };

}

