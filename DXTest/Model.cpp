#include "Model.h"
#include "DXSandbox.h"
#include "Utils.h"
#include "texture.h"
#include "SimpleMath.h"
#include "DXTestException.h"

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
: mInitialized(false),
  mEnabled(true),
  mpVertexBuffer(nullptr),
  mpIndexBuffer(nullptr),
  mVertexCount(0),
  mIndexCount(0),
  mpMesh(nullptr),
  mpMeshIndices(nullptr),
  mpTexture(nullptr),
  mPosition(0, 0, 0),
  mColor(1, 1, 1, 1),
  mBoundingSphereRadius(2.0f)
{
}

Model::~Model()
{
	Shutdown();
}

void Model::Initialize(ID3D11Device *pDevice, const std::string& modelFile, const std::string& textureFile)
{
	if (mInitialized)
	{
		return;
	}

	VerifyNotNull(pDevice);
    LoadModel(modelFile);
	InitializeBuffers(pDevice);
	LoadTexture(pDevice, textureFile);

	mInitialized = true;
}

void Model::InitializeBuffers(ID3D11Device *pDevice)
{
	AssertNotNull(pDevice);
	AssertIsFalse(mInitialized);

	// Create two arrays to temporarily hold vertex and index data.
	vertex_type_t * pVertices = new vertex_type_t[mVertexCount];
	unsigned long * pIndices = new unsigned long[mIndexCount];

	// Fill the temporary arrays with our vertex and index data.
	//  - NOTE: Vertices need to be in clock wise order.
    for (int i = 0; i < mVertexCount; ++i)
    {
        pVertices[i].position = Vector3(mpMesh[i].x, mpMesh[i].y, mpMesh[i].z);
        pVertices[i].texture = Vector2(mpMesh[i].tu, mpMesh[i].tv);
        pVertices[i].normal = Vector3(mpMesh[i].nx, mpMesh[i].ny, mpMesh[i].nz);

//        pIndices[i] = i;
    }

    for (int i = 0; i < mIndexCount; ++i)
    {
        pIndices[i] = static_cast<unsigned long>(mpMeshIndices[i]);
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

	vertexData.pSysMem = pVertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Create the vertex buffer.
	HRESULT result = pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &mpVertexBuffer);
	
	VerifyDXResult(result);
	VerifyNotNull(mpVertexBuffer);

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

	indexData.pSysMem = pIndices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = pDevice->CreateBuffer(&indexBufferDesc, &indexData, &mpIndexBuffer);

	VerifyDXResult(result);
	VerifyNotNull(mpIndexBuffer);

	// Release temporary vertex and index buffers.
	SafeDeleteArray(pVertices);
	SafeDeleteArray(pIndices);
}

// TODO: Vastly improve this code loading.
void Model::LoadModel(const std::string& filepath)
{
    if (Utils::EndsWith(filepath, ".txt"))
    {
        LoadTxtModelv1(filepath);
    }
    else
    {
        LoadTxtModelv2(filepath);
    }
}

// TODO: Vastly improve this code loading.
void Model::LoadTxtModelv1(const std::string& filepath)
{
    // Load the text file containing mesh data.
    std::ifstream meshStream(filepath.c_str());

    if (meshStream.fail())
    {
        throw FileLoadException(Utils::ConvertUtf8ToWString(filepath));
    }

    // Get the vertex count.
    meshStream >> mVertexCount;
    mIndexCount = mVertexCount;

    // Read the vertex buffer into memory.
    mpMesh = new Model::s_mesh_vertex_t[mVertexCount];
    mpMeshIndices = new int[mVertexCount];

    for (int i = 0; i < mVertexCount; ++i)
    {
        meshStream >> mpMesh[i].x >> mpMesh[i].y >> mpMesh[i].z;
        meshStream >> mpMesh[i].tu >> mpMesh[i].tv;
        meshStream >> mpMesh[i].nx >> mpMesh[i].ny >> mpMesh[i].nz;

        mpMeshIndices[i] = i;
    }

    // All done.
    meshStream.close();
}

// TODO: Vastly improve this code loading.
void Model::LoadTxtModelv2(const std::string& filepath)
{
    // Load the text file containing mesh data.
    std::ifstream meshStream(filepath.c_str());

    if (meshStream.fail())
    {
        throw FileLoadException(Utils::ConvertUtf8ToWString(filepath));
    }
    
    // Get mesh header.
    std::string fileType;
    meshStream >> fileType >> mVertexCount >> mIndexCount;

    // Read the vertex buffer into memory.
    mpMesh = new Model::s_mesh_vertex_t[mVertexCount];

    for (int i = 0; i < mVertexCount; ++i)
    {
        meshStream >> mpMesh[i].x >> mpMesh[i].y >> mpMesh[i].z;
        meshStream >> mpMesh[i].tu >> mpMesh[i].tv;
        meshStream >> mpMesh[i].nx >> mpMesh[i].ny >> mpMesh[i].nz;
    }

    // Read the index buffer into memory.
    mpMeshIndices = new int[mIndexCount];

    for (int i = 0; i < mIndexCount; ++i)
    {
        meshStream >> mpMeshIndices[i];
    }

    // All done.
    meshStream.close();
}

void Model::LoadTexture(ID3D11Device *pDevice, const std::string& filepath)
{
	mpTexture = new Texture();
    mpTexture->Initialize(pDevice, Utils::ConvertUtf8ToWString(filepath));
}

void Model::Shutdown()
{
	if (!mInitialized)
	{
		return;
	}

	ShutdownBuffers();
	ReleaseTexture();
    ReleaseModel();

	mInitialized = false;
}

void Model::ShutdownBuffers()
{
	SafeRelease(mpIndexBuffer);
	SafeRelease(mpVertexBuffer);
}

void Model::ReleaseTexture()
{
	SafeDelete(mpTexture);
}

void Model::ReleaseModel()
{
    SafeDeleteArray(mpMesh);
    SafeDeleteArray(mpIndexBuffer);
}


// TODO: This needs to be combined with the shader render logic.
//  - Right now this method is better called "BindModelBuffersForRendering".
void Model::Render(ID3D11DeviceContext *pDeviceContext)
{
	VerifyNotNull(pDeviceContext);
	RenderBuffers(pDeviceContext);
}

void Model::RenderBuffers(ID3D11DeviceContext *pContext)
{
	AssertNotNull(pContext);

	unsigned int stride = sizeof(vertex_type_t);
	unsigned int offset = 0;

	// Activate vertex and index buffers object for rendering.
	pContext->IASetVertexBuffers(0, 1, &mpVertexBuffer, &stride, &offset);
	pContext->IASetIndexBuffer(mpIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Render the model using triangle primitives.
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

int Model::GetIndexCount() const
{
	return mIndexCount;
}

ID3D11ShaderResourceView * Model::GetTexture()
{
	return mpTexture->GetTexture();
}