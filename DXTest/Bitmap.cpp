#include "Bitmap.h"
#include "DXSandbox.h"
#include "Utils.h"
#include "texture.h"
#include "SimpleMath.h"
#include "DXTestException.h"

#include <d3d11.h>
#include <string>
#include <fstream>

using namespace DirectX::SimpleMath;

struct bitmap_vertex_t
{
    Vector3 position;
    Vector2 texture;
};

Bitmap::Bitmap()
: mpVertexBuffer(nullptr),
  mpIndexBuffer(nullptr),
  mVertexCount(0),
  mIndexCount(0),
  mpTexture(nullptr),
  mScreenWidth(0),
  mScreenHeight(0),
  mBitmapWidth(0),
  mBitmapHeight(0),
  mPreviousX(-1),
  mPreviousY(-1)
{
}

Bitmap::~Bitmap()
{
}

void Bitmap::Initialize(
    ID3D11Device * pDevice,
    int screenWidth,
    int screenHeight,
    const std::string& textureFile,
    int bitmapWidth,
    int bitmapHeight)
{
    VerifyNotNull(pDevice);

    // Store screen and bitmap size information.
    mScreenWidth = screenWidth;
    mScreenHeight = screenHeight;
    mBitmapWidth = bitmapWidth;
    mBitmapHeight = bitmapHeight;

    // Initialize vertex and index buffers.
    InitializeBuffers(pDevice);

    // Load bitmap texture.
    LoadTexture(pDevice, textureFile);

    SetInitialized();
}

void Bitmap::InitializeBuffers(ID3D11Device *pDevice)
{
    // Set the number of vertices and indices for this bitmap.
    mVertexCount = 6;
    mIndexCount = mVertexCount;

    // Create the vertex and index array.
    bitmap_vertex_t * pVertices = new bitmap_vertex_t[mVertexCount];
    unsigned long * pIndices = new unsigned long[mIndexCount];

    // Set vertex buffer to zero.
    memset(pVertices, 0, sizeof(bitmap_vertex_t)* mVertexCount);    // TODO: do this better.

    // Set index buffer to point to each vertex in order.
    for (int i = 0; i < mIndexCount; ++i)
    {
        pIndices[i] = i;            // TODO: Isn't there an STL algorithm for this?
    }

    // Set up the dynamic vertex buffer description.
    D3D11_BUFFER_DESC vbd;

    vbd.Usage = D3D11_USAGE_DYNAMIC;
    vbd.ByteWidth = sizeof(bitmap_vertex_t)* mVertexCount;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vbd.MiscFlags = 0;
    vbd.StructureByteStride = 0;

    // Create the dynamic vertex buffer.
    D3D11_SUBRESOURCE_DATA vertexData;

    vertexData.pSysMem = pVertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    HRESULT result = pDevice->CreateBuffer(&vbd, &vertexData, &mpVertexBuffer);

    VerifyDXResult(result);
    VerifyNotNull(mpVertexBuffer);

    // Set up the static index buffer description.
    D3D11_BUFFER_DESC ibd;

    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = sizeof(unsigned long)* mIndexCount;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    ibd.StructureByteStride = 0;

    // Create the static index buffer.
    D3D11_SUBRESOURCE_DATA indexData;

    indexData.pSysMem = pIndices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    result = pDevice->CreateBuffer(&ibd, &indexData, &mpIndexBuffer);

    VerifyDXResult(result);
    VerifyNotNull(mpIndexBuffer);

    // Release temporary resources.
    SafeDeleteArray(pVertices);
    SafeDeleteArray(pIndices);
}

void Bitmap::OnShutdown()
{
    ReleaseTexture();
    ShutdownBuffers();
}

void Bitmap::ReleaseTexture()
{
    SafeDelete(mpTexture);
}

void Bitmap::ShutdownBuffers()
{
    SafeRelease(mpIndexBuffer);
    SafeRelease(mpVertexBuffer);
}

void Bitmap::Render(ID3D11DeviceContext *pDeviceContext, int x, int y)
{
    VerifyNotNull(pDeviceContext);

    // Update vertex/index buffer for our new bitmap position and then render the bitmap.
    //  TODO: The caching logic should be brought out here instead of inside the function.
    //  TODO: Don't rebuild the vertex/index buffers. Use shader constants instead?
    UpdateBuffers(pDeviceContext, x, y);
    RenderBuffers(pDeviceContext);
}

// Called each frame to update contents of dynamic vertex buffer which repositions the bitmap.
void Bitmap::UpdateBuffers(ID3D11DeviceContext *pDeviceContext, int x, int y)
{
    // Check if the position we are rendering to has not changed since the last update.
    //  TODO: Use dirty flag + Add Set/GetPosition or something.
    if (mPreviousX == x && mPreviousY == y)
    {
        return;
    }
    else
    {
        mPreviousX = x;
        mPreviousY = y;
    }

    // Calculate the rectangle dimensions of the bitmap rect relative to the screen rectangle. In DirectX, the middle
    // of the screen is (0,0). This means the boundaries of the screen rect will change depending on screen size, and
    // opposite sides will be mirrored values (eg, w and -w).
    float left = (static_cast<float>(mScreenWidth) / 2.0f * -1.0f) + static_cast<float>(x);
    float right = left + static_cast<float>(mBitmapWidth);
    float top = (static_cast<float>(mScreenHeight) / 2.0f) - static_cast<float>(y);
    float bottom = top - static_cast<float>(mBitmapHeight);

    // Create a software vertex buffer for the new buffer.
    //  TODO: Don't dynamically allocate this vertex buffer every time bitmap moves!!! Use static buffer or something.
    //  TODO: Don't have to do above. Just map vbuffer into memory and write directly to it. Duh.
    bitmap_vertex_t * pVertices = new bitmap_vertex_t[mVertexCount];

    // first triangle.
    pVertices[0].position = Vector3(left, top, 0.0f);   // top left.
    pVertices[0].texture = Vector2(0.0f, 0.0f);

    pVertices[1].position = Vector3(right, bottom, 0.0f);   // bottom right.
    pVertices[1].texture = Vector2(1.0f, 1.0f);

    pVertices[2].position = Vector3(left, bottom, 0.0f);    // bottom left.
    pVertices[2].texture = Vector2(0.0f, 1.0f); 

    // second triangle.
    pVertices[3].position = Vector3(left, top, 0.0f);       // top left.
    pVertices[3].texture = Vector2(0.0f, 0.0f);

    pVertices[4].position = Vector3(right, top, 0.0f);      // top right.
    pVertices[4].texture = Vector2(1.0f, 0.0f);

    pVertices[5].position = Vector3(right, bottom, 0.0f);   // bottom right.
    pVertices[5].texture = Vector2(1.0f, 1.0f);

    // Lock the vertex buffer on the GPU and map it into system memory for writing.
    D3D11_MAPPED_SUBRESOURCE resource;

    HRESULT result = pDeviceContext->Map(mpVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

    VerifyDXResult(result);
    VerifyNotNull(resource.pData);

    // Write the new vertex buffer values.
    bitmap_vertex_t * pVerts = reinterpret_cast<bitmap_vertex_t*>(resource.pData);
    memcpy(pVerts, reinterpret_cast<void*>(pVertices), sizeof(bitmap_vertex_t)* mVertexCount); // TODO: NO!

    // Unlock the vertex buffer, and release the temporary software array.
    pDeviceContext->Unmap(mpVertexBuffer, 0);
    SafeDeleteArray(pVertices);
}

void Bitmap::RenderBuffers(ID3D11DeviceContext *pDeviceContext)
{
    unsigned int stride = sizeof(bitmap_vertex_t);
    unsigned int offset = 0;

    // Activate the bitmap vertex and index buffers for rendering.
    pDeviceContext->IASetVertexBuffers(0, 1, &mpVertexBuffer, &stride, &offset);
    pDeviceContext->IASetIndexBuffer(mpIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Bitmap::LoadTexture(ID3D11Device *pDevice, const std::string& filename)
{
    mpTexture = new Texture();
    mpTexture->InitializeFromFile(pDevice, Utils::ConvertUtf8ToWString(filename));
}

ID3D11ShaderResourceView * Bitmap::GetTexture() const
{
    // TODO: Should return the texture! Or hold on the the shader resource view instance and return that.
    return mpTexture->GetTexture();
}

int Bitmap::GetIndexCount() const
{
    return mIndexCount;
}