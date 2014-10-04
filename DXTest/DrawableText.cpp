#include "DrawableText.h"
#include "Font.h"
#include "FontShader.h"
#include "DXSandbox.h"
#include "Dx3d.h"
#include "DrawableText.h"
#include "Size.h"

#include "SimpleMath.h"
#include <d3d11.h>

#include <string>
#include <vector>

using namespace DirectX::SimpleMath;

DrawableText::DrawableText()
    : mMaxLength(0),
      mIndexCount(0),
      mVertexCount(0),
      mRed(0.0f),
      mGreen(0.0f),
      mBlue(0.0f),
      mIndexBuffer(),
      mVertexBuffer()
{
}

DrawableText::~DrawableText()
{
}

void DrawableText::Initialize(Dx3d& dx, int maxLength)
{
    // Initialize values in text object.
    mMaxLength = maxLength;
    mVertexCount = 6 * maxLength;       // TODO: Magic number.
    mIndexCount = mVertexCount;

    // Allocate temporary software vertex and index arrays.
    vertex_t * pVertices = new vertex_t[mVertexCount];
    unsigned long * pIndices = new unsigned long[mIndexCount];

    // Initialize vertex array to zero, and the index array to point to each vertex in order.
    memset(pVertices, 0, sizeof(vertex_t)* mVertexCount);

    for (unsigned int i = 0; i < mIndexCount; ++i)
    {
        pIndices[i] = i;
    }

    // Create a dynamic vertex buffer to store the text vertices. This allows us to update the contentents of the
    // text at any time.
    D3D11_BUFFER_DESC vbd;

    vbd.Usage = D3D11_USAGE_DYNAMIC;
    vbd.ByteWidth = sizeof(vertex_t)* mVertexCount;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vbd.MiscFlags = 0;
    vbd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vertexData;

    vertexData.pSysMem = pVertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    HRESULT result = dx.GetDevice()->CreateBuffer(&vbd, &vertexData, &mVertexBuffer);
    VerifyDXResult(result);

    // Create a static index buffer. It is static because we do not need to reorder the vertices of the text mesh.
    D3D11_BUFFER_DESC ibd;

    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = sizeof(unsigned long)* mIndexCount;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    ibd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA indexData;

    indexData.pSysMem = pIndices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    result = dx.GetDevice()->CreateBuffer(&ibd, &indexData, &mIndexBuffer);
    VerifyDXResult(result);

    // Release temporary arrays now that we have created the hardware buffers.
    SafeDeleteArray(pVertices);
    SafeDeleteArray(pIndices);

    SetInitialized();
}

void DrawableText::OnShutdown()
{

}

void DrawableText::Update(
    Dx3d& dx,
    Font& font,
    const std::string& inputText,
    const Size& screenSize,
    int x, int y,
    float r, float g, float b)
{
    Verify(static_cast<unsigned int>(mMaxLength) > inputText.size());

    // Store the requested color.
    mRed = r;
    mGreen = g;
    mBlue = b;

    // Create a temporary vertex array.
    //  TODO: Just use the mapped hardware array.
    vertex_t * pVertices = new vertex_t[mVertexCount];
    memset(pVertices, 0, sizeof(vertex_t) * mVertexCount);

    // Calculate the x and y pixel position on screen to draw at.
    float drawX = static_cast<float>(((screenSize.width / 2.0f) * -1.0f) + x);
    float drawY = static_cast<float>(((screenSize.height / 2.0f) - y));

    // Use the font class to build the vertex array.
    font.BuildVertexArray(reinterpret_cast<void*>(pVertices), inputText, drawX, drawY);

    // Lock the vertex buffer and copy the new vertex array into that buffer.
    D3D11_MAPPED_SUBRESOURCE resource;
    HRESULT result = dx.GetDeviceContext()->Map(mVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

    VerifyDXResult(result);
    VerifyNotNull(resource.pData);

    vertex_t * pVerts = reinterpret_cast<vertex_t*>(resource.pData);
    memcpy(pVerts, reinterpret_cast<void*>(pVertices), sizeof(vertex_t)* mVertexCount);

    dx.GetDeviceContext()->Unmap(mVertexBuffer.Get(), 0);

    // Clean up no longer needed vertex array.
    SafeDeleteArray(pVertices);
}

void DrawableText::Render(Dx3d& dx,
                          Font& font,
                          FontShader& fontShader,
                          const DirectX::SimpleMath::Matrix& baseViewMatrix,
                          const DirectX::SimpleMath::Matrix& worldMatrix,
                          const DirectX::SimpleMath::Matrix& orthoMatrix) const
{
    unsigned int stride = sizeof(vertex_t);
    unsigned int offset = 0;

    // Activate the vertex and index buffers for rendering.
    ID3D11Buffer * vertexBuffers[1] = { mVertexBuffer.Get() };

    dx.GetDeviceContext()->IASetVertexBuffers(0, 1, vertexBuffers, &stride, &offset);
    dx.GetDeviceContext()->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    dx.GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Render the text by way of shader. TODO: Don't do it like this.
    Vector4 pixelColor(mRed, mGreen, mBlue, 1.0f);

    fontShader.Render(dx,
                         mIndexCount,
                         worldMatrix,
                         baseViewMatrix,
                         orthoMatrix,
                         font.GetTexture(),
                         pixelColor);
}