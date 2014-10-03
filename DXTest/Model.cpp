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

struct vertex_type_t
{
	Vector3 position;
	Vector2 texture;
    Vector3 normal;
};

// TODO: Not sure why the author all of a sudden started using functions, only to have them be empty shells to
// forward methods calls to. Merge them together, and then consider splitting them correctly.

Model::Model()
: mEnabled(true),
  mVertexCount(0u),
  mIndexCount(0u),
  mVertexBuffer(),
  mIndexBuffer(),
  mMeshVertices(),
  mMeshIndices(),
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

    LoadModel(modelFile);
	InitializeBuffers(pDevice);
	
    // Load requested texture.
    mTexture.reset(new Texture());
    mTexture->Initialize(pDevice, textureFile);

    SetInitialized();
}

void Model::InitializeBuffers(ID3D11Device *pDevice)
{
	AssertNotNull(pDevice);

	// Create two arrays to temporarily hold vertex and index data.
	std::vector<vertex_type_t> vertices(mVertexCount);
	std::vector<unsigned long> indices(mIndexCount);

	// Fill the temporary arrays with our vertex and index data.
	//  - NOTE: Vertices need to be in clock wise order.
    for (unsigned int i = 0; i < mVertexCount; ++i)
    {
        vertices[i].position = Vector3(mMeshVertices[i].x, mMeshVertices[i].y, mMeshVertices[i].z);
        vertices[i].texture = Vector2(mMeshVertices[i].tu, mMeshVertices[i].tv);
        vertices[i].normal = Vector3(mMeshVertices[i].nx, mMeshVertices[i].ny, mMeshVertices[i].nz);
    }

    for (unsigned int i = 0; i < mIndexCount; ++i)
    {
        indices[i] = static_cast<unsigned long>(mMeshIndices[i]);
    }

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
void Model::LoadModel(const std::wstring& filepath)
{
    if (Utils::EndsWith(filepath, L".txt"))
    {
        LoadTxtModelv1(filepath);
    }
    else
    {
        LoadTxtModelv2(filepath);
    }
}

// TODO: Vastly improve this code loading.
void Model::LoadTxtModelv1(const std::wstring& filepath)
{
    // Load the text file containing mesh data.
    std::ifstream meshStream(filepath.c_str());

    if (meshStream.fail())
    {
        throw FileLoadException(filepath);
    }

    // Get the vertex count.
    meshStream >> mVertexCount;
    mIndexCount = mVertexCount;

    // Read the vertex buffer into memory.
    mMeshVertices.resize(mVertexCount);
    mMeshIndices.resize(mVertexCount);

    for (unsigned int i = 0; i < mVertexCount; ++i)
    {
        meshStream >> mMeshVertices[i].x >> mMeshVertices[i].y >> mMeshVertices[i].z;
        meshStream >> mMeshVertices[i].tu >> mMeshVertices[i].tv;
        meshStream >> mMeshVertices[i].nx >> mMeshVertices[i].ny >> mMeshVertices[i].nz;

        mMeshIndices[i] = i;
    }

    // All done.
    meshStream.close();
}

// TODO: Vastly improve this code loading.
void Model::LoadTxtModelv2(const std::wstring& filepath)
{
    // Load the text file containing mesh data.
    // Utils::ConvertUtf8ToWString(
    std::ifstream meshStream(filepath.c_str());

    if (meshStream.fail())
    {
        throw FileLoadException(filepath);
    }
    
    // Get mesh header.
    std::string fileType;
    meshStream >> fileType >> mVertexCount >> mIndexCount;

    // Read the vertex buffer into memory.
    mMeshVertices.resize(mVertexCount);

    for (unsigned int i = 0; i < mVertexCount; ++i)
    {
        meshStream >> mMeshVertices[i].x >> mMeshVertices[i].y >> mMeshVertices[i].z;
        meshStream >> mMeshVertices[i].tu >> mMeshVertices[i].tv;
        meshStream >> mMeshVertices[i].nx >> mMeshVertices[i].ny >> mMeshVertices[i].nz;
    }

    // Read the index buffer into memory.
    mMeshIndices.resize(mIndexCount);

    for (unsigned int i = 0; i < mIndexCount; ++i)
    {
        meshStream >> mMeshIndices[i];
    }

    // All done.
    meshStream.close();
}

void Model::OnShutdown()
{
}

// TODO: This needs to be combined with the shader render logic.
//  - Right now this method is better called "BindModelBuffersForRendering".
void Model::Render(ID3D11DeviceContext *pDeviceContext)
{
    if (!IsInitialized()) { throw NotInitializedException(L"Model"); }
	VerifyNotNull(pDeviceContext);
	RenderBuffers(pDeviceContext);
}

void Model::RenderBuffers(ID3D11DeviceContext *pContext)
{
	AssertNotNull(pContext);

	unsigned int stride = sizeof(vertex_type_t);
	unsigned int offset = 0;

	// Activate vertex and index buffers object for rendering.
    ID3D11Buffer* vertexBuffers[1] = { mVertexBuffer.Get() };

    pContext->IASetVertexBuffers(0, 1, vertexBuffers, &stride, &offset);
    pContext->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// Render the model using triangle primitives.
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

ID3D11ShaderResourceView * Model::GetTexture()
{
	return mTexture->GetTexture();
}