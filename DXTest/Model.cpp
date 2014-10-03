#include "Model.h"
#include "DXSandbox.h"
#include "Utils.h"
#include "texture.h"
#include "SimpleMath.h"
#include "DXTestException.h"

#include <vector>
#include <d3d11.h>
#include <string>
#include <fstream>

using namespace DirectX::SimpleMath;

// TODO: Split the s_mesh* and LoadModel code into a new class called SoftwareMesh (or something).
//  - It holds mesh data loaded from disk.
//  - Can load / save mesh data.
//  - Contains type info to assist in coverted from it to DX hardware mesh for uploading

struct vertex_type_t
{
	Vector3 position;
	Vector2 texture;
    Vector3 normal;
};

Model::Model()
: mEnabled(true),
  mVertexCount(0u),
  mIndexCount(0u),
  mVertexBuffer(),
  mIndexBuffer(),
  mTexture(),
  mPosition(0, 0, 0),
  mColor(1, 1, 1, 1),
  mBoundingSphereRadius(2.0f)
{
}

Model::~Model()
{
}

void Model::Initialize(
    ID3D11Device *pDevice,
    const std::wstring& modelFile,
    const std::wstring& textureFile)
{
	if (IsInitialized()) { return; }
	VerifyNotNull(pDevice);

    s_mesh_data_t meshData;
    LoadModel(modelFile, &meshData);

    // TODO: Remember to set vertex/index count somewhere now that we put it into smesh_data_T.
    InitializeBuffers(pDevice, meshData);

    // Load requested texture.
    mTexture.reset(new Texture());
    mTexture->Initialize(pDevice, textureFile);

    SetInitialized();
}

void Model::InitializeBuffers(
    ID3D11Device *pDevice,
    const s_mesh_data_t& meshData)  // TODO: Make this const, set values from caller makes more sense. less state.
{
	AssertNotNull(pDevice);

	// Create two arrays to temporarily hold vertex and index data.
    //  TODO: Explain conversion, uploading, etc.
    std::vector<vertex_type_t> vertices(meshData.vertices.size());
    std::vector<unsigned long> indices(meshData.indices.size());

	// Fill the temporary arrays with our vertex and index data.
	//  - NOTE: Vertices need to be in clock wise order.
    for (unsigned int i = 0; i < meshData.vertices.size(); ++i)
    {
        vertices[i].position = Vector3(meshData.vertices[i].x, meshData.vertices[i].y, meshData.vertices[i].z);
        vertices[i].texture = Vector2(meshData.vertices[i].tu, meshData.vertices[i].tv);
        vertices[i].normal = Vector3(meshData.vertices[i].nx, meshData.vertices[i].ny, meshData.vertices[i].nz);
    }

    for (unsigned int i = 0; i < meshData.indices.size(); ++i)
    {
        indices[i] = static_cast<unsigned long>(meshData.indices[i]);
    }

    mVertexCount = meshData.vertices.size();
    mIndexCount = meshData.indices.size();

	// Set up static vertex buffer description.
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(vertex_type_t)* mVertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Set up subresource structure with a pointer to the vertex data.
	D3D11_SUBRESOURCE_DATA vertexData;
	ZeroMemory(&vertexData, sizeof(vertexData));

	vertexData.pSysMem = &vertices[0];
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Create the vertex buffer.
	HRESULT result = pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &mVertexBuffer);
	
	VerifyDXResult(result);

	// Set up the static index buffer description.
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long)* mIndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Set up subresource structure with a pointer to the index data.
	D3D11_SUBRESOURCE_DATA indexData;
	ZeroMemory(&indexData, sizeof(indexData));

	indexData.pSysMem = &indices[0];
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = pDevice->CreateBuffer(&indexBufferDesc, &indexData, &mIndexBuffer);

	VerifyDXResult(result);
}

// TODO: Vastly improve this code loading.
void Model::LoadModel(const std::wstring& filepath, s_mesh_data_t *pMeshDataOut) const
{
    AssertNotNull(pMeshDataOut);

    if (Utils::EndsWith(filepath, L".txt"))
    {
        LoadTxtModelv1(filepath, pMeshDataOut);
    }
    else
    {
        LoadTxtModelv2(filepath, pMeshDataOut);
    }
}

// TODO: Vastly improve this code loading.
void Model::LoadTxtModelv1(const std::wstring& filepath, s_mesh_data_t *pMeshDataOut) const
{
    AssertNotNull(pMeshDataOut);

    // Load the text file containing mesh data.
    std::ifstream meshStream(filepath.c_str());

    if (meshStream.fail())
    {
        throw FileLoadException(filepath);
    }

    // Get the vertex count. (Index count is the same since one to one mapping).
    unsigned int vertexCount;
    meshStream >> vertexCount;

    // Read the vertex buffer into memory.
    pMeshDataOut->vertices.resize(vertexCount);
    pMeshDataOut->indices.resize(vertexCount);

    std::vector<s_mesh_vertex_t>& verts = pMeshDataOut->vertices;   // alias to reduce typing.
    std::vector<int>& indices = pMeshDataOut->indices;              // alias to reduce typing.

    for (unsigned int i = 0; i < vertexCount; ++i)
    {
        meshStream >> verts[i].x >> verts[i].y >> verts[i].z;
        meshStream >> verts[i].tu >> verts[i].tv;
        meshStream >> verts[i].nx >> verts[i].ny >> verts[i].nz;

        indices[i] = i;
    }

    meshStream.close();
}

// TODO: Vastly improve this code loading.
void Model::LoadTxtModelv2(const std::wstring& filepath, s_mesh_data_t *pMeshDataOut) const
{
    AssertNotNull(pMeshDataOut);

    // Load the text file containing mesh data.
    // Utils::ConvertUtf8ToWString(
    std::ifstream meshStream(filepath.c_str());

    if (meshStream.fail())
    {
        throw FileLoadException(filepath);
    }
    
    // Get mesh header.
    std::string fileType;
    unsigned int vertexCount = 0u, indexCount = 0u;

    meshStream >> fileType >> vertexCount >> indexCount;

    // Read the vertex buffer into memory.
    std::vector<s_mesh_vertex_t>& verts = pMeshDataOut->vertices;   // alias to reduce typing.
    verts.resize(vertexCount);

    for (unsigned int i = 0; i < vertexCount; ++i)      // TODO: for each
    {
        meshStream >> verts[i].x >> verts[i].y >> verts[i].z;
        meshStream >> verts[i].tu >> verts[i].tv;
        meshStream >> verts[i].nx >> verts[i].ny >> verts[i].nz;
    }

    // Read the index buffer into memory.
    std::vector<int>& indices = pMeshDataOut->indices;              // alias to reduce typing.
    indices.resize(indexCount);

    for (unsigned int i = 0; i < indexCount; ++i)      // TODO: for each
    {
        meshStream >> indices[i];
    }

    // All done.
    meshStream.close();
}

void Model::OnShutdown()
{
    // Empty.
}

// TODO: This needs to be combined with the shader render logic.
//  - Right now this method is better called "BindModelBuffersForRendering".
void Model::Render(ID3D11DeviceContext *pDeviceContext)
{
    if (!IsInitialized()) { throw NotInitializedException(L"Model"); }
	VerifyNotNull(pDeviceContext);

    unsigned int stride = sizeof(vertex_type_t);
    unsigned int offset = 0;

    // Activate vertex and index buffers object for rendering.
    ID3D11Buffer* vertexBuffers[1] = { mVertexBuffer.Get() };

    pDeviceContext->IASetVertexBuffers(0, 1, vertexBuffers, &stride, &offset);
    pDeviceContext->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

    // Render the model using triangle primitives.
    pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

ID3D11ShaderResourceView * Model::GetTexture()
{
	return mTexture->GetTexture();
}