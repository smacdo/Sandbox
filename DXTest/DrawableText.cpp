#include "DrawableText.h"
#include "Font.h"
#include "FontShader.h"
#include "DXSandbox.h"
#include "Dx3d.h"
#include "DrawableText.h"
#include "Size.h"
#include "Camera.h"

#include "SimpleMath.h"
#include <d3d11.h>

#include <string>
#include <vector>
#include <algorithm>

using namespace DirectX::SimpleMath;

DrawableText::DrawableText(const std::string& name)
    : mName(name),
      mColor(),
      mMaxLength(0),
      mIndexCount(0),
      mVertexCount(0),
      mTextIndexBuffer(),
      mTextVertexBuffer()
{
}

DrawableText::~DrawableText()
{
}

void DrawableText::Initialize(Dx3d& dx, int maxLength)
{
    // Initialize values in text object.
    unsigned int textVertexCount = maxLength * 6;        // TODO: Explain why 6.
    unsigned int textIndexCount = textVertexCount;       // Because tutorial code, that's why.

    mMaxLength = maxLength;             // TODO: set to zero, only set value if succeeded.
    mVertexCount = 6 * maxLength;       // TODO: Magic number.
    mIndexCount = mVertexCount;         // TODO: Same as above two

    // Create a dynamic vertex buffer and static index buffer. They will hold the vertices to a set of rectangles, and
    // each rectangle represents a textured letter.
    Microsoft::WRL::ComPtr<ID3D11Buffer> textVertexBuffer;

    HRESULT hr = CreateTextVertexBuffer(dx, textVertexCount, &textVertexBuffer);

    if (SUCCEEDED(hr))
    {
        Microsoft::WRL::ComPtr<ID3D11Buffer> textIndexBuffer;
        hr = CreateTextIndexBuffer(dx, textIndexCount, &textIndexBuffer);

        if (SUCCEEDED(hr))
        {
            mTextVertexBuffer = textVertexBuffer.Detach();
            mTextIndexBuffer = textIndexBuffer.Detach();
        }
    }
    
    VerifyDXResult(hr);         // Check for fatal errors.
    SetInitialized();
}

// Creates a dynamic vertex buffer to store the text vertices. This allows us to update the renderable text at any
// point.
HRESULT DrawableText::CreateTextVertexBuffer(
    Dx3d& dx,
    unsigned int vertexCount,
    ID3D11Buffer **ppVertexBufferOut) const
{
    VerifyNotNull(ppVertexBufferOut);
    *ppVertexBufferOut = nullptr;

    // Allocate temporary array to hold the software generated vertices.
    //  TODO: Do we need to do this or can we just write all the vertices to zero w/out this array?
    std::vector<vertex_t> vertices(vertexCount);

    // Describe the vertex buffer we want to create.
    D3D11_BUFFER_DESC vbd;

    vbd.Usage = D3D11_USAGE_DYNAMIC;
    vbd.ByteWidth = sizeof(vertex_t)* vertexCount;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vbd.MiscFlags = 0;
    vbd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vertexData;

    vertexData.pSysMem = &vertices[0];
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    // Try to create the vertex buffer, and let the caller know the results of the operation.
    return dx.GetDevice()->CreateBuffer(&vbd, &vertexData, ppVertexBufferOut);
}

// Creates a static index buffer to store the text vertex indices. Index buffer is static since the backing mesh will
// not need to be regenerated at any point.
HRESULT DrawableText::CreateTextIndexBuffer(
    Dx3d& dx,
    unsigned int indexCount,
    ID3D11Buffer **ppIndexBufferOut) const
{
    VerifyNotNull(ppIndexBufferOut);
    *ppIndexBufferOut = nullptr;

    // Generate an index buffer that makes each entry point to the corresponding vertex in order.
    //  (This is from rastertek tutorial code... again not very sophisticated).
    std::vector<unsigned long> indices(indexCount, 0);

    unsigned long nextIndex = 0;
    std::generate(indices.begin(), indices.end(), [&nextIndex]() { return nextIndex++; });

    // Describe the index buffer we want to create.
    D3D11_BUFFER_DESC ibd;

    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = sizeof(unsigned long)* indexCount;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    ibd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA indexData;

    indexData.pSysMem = &indices[0];
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    // Try to create the index buffer, and let the caller know the results of the operation.
    return dx.GetDevice()->CreateBuffer(&ibd, &indexData, ppIndexBufferOut);
}

void DrawableText::OnShutdown()
{

}

void DrawableText::Update(
    Dx3d& dx,
    Font& font,
    const std::string& inputText,
    const Size& screenSize,
    const DirectX::SimpleMath::Vector2& position,
    const Color& color)
{
    Verify(static_cast<unsigned int>(mMaxLength) > inputText.size());
    mColor = color;

    // Create a temporary vertex array.
    //  TODO: Just use the mapped hardware array.
    vertex_t * pVertices = new vertex_t[mVertexCount];
    memset(pVertices, 0, sizeof(vertex_t) * mVertexCount);

    // Calculate the x and y pixel position on screen to draw at.
    float drawX = static_cast<float>(((screenSize.width / 2.0f) * -1.0f) + position.x);
    float drawY = static_cast<float>(((screenSize.height / 2.0f) - position.y));

    // Use the font class to build the vertex array.
    font.BuildVertexArray(reinterpret_cast<void*>(pVertices), inputText, drawX, drawY);

    // Lock the vertex buffer and copy the new vertex array into that buffer.
    D3D11_MAPPED_SUBRESOURCE resource;
    HRESULT result = dx.GetDeviceContext()->Map(mTextVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

    VerifyDXResult(result);
    VerifyNotNull(resource.pData);

    vertex_t * pVerts = reinterpret_cast<vertex_t*>(resource.pData);
    memcpy(pVerts, reinterpret_cast<void*>(pVertices), sizeof(vertex_t)* mVertexCount);

    dx.GetDeviceContext()->Unmap(mTextVertexBuffer.Get(), 0);

    // Clean up no longer needed vertex array.
    SafeDeleteArray(pVertices);
}

void DrawableText::Render(Dx3d& dx,
                          Font& font,
                          FontShader& fontShader,
                          const Camera& camera,
                          const DirectX::SimpleMath::Matrix& worldMatrix) const
{
    unsigned int stride = sizeof(vertex_t);
    unsigned int offset = 0;

    // Activate the vertex and index buffers for rendering.
    ID3D11Buffer * vertexBuffers[1] = { mTextVertexBuffer.Get() };

    dx.GetDeviceContext()->IASetVertexBuffers(0, 1, vertexBuffers, &stride, &offset);
    dx.GetDeviceContext()->IASetIndexBuffer(mTextIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    dx.GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Render the text by way of shader. TODO: Don't do it like this.
    fontShader.Render(dx,
                      mIndexCount,
                      worldMatrix,
                      camera.ViewMatrix(),
                      camera.OrthoMatrix(),
                      font.GetTexture(),
                      mColor.ToVector4());          // TODO: Make font shader accept color type.
}