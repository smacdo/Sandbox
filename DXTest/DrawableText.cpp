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

// TODO: Don't encode position into the vertex data. Pass it as a shader param.

using namespace DirectX::SimpleMath;
const unsigned int RectVertexCount = 6;
const float CharacterSpacing = 1.0f;
const float CharacterHeight = 16;

DrawableText::DrawableText(const std::string& name)
    : mName(name),
      mColor(),
      mMaxTextLength(0),
      mIndexCount(0),
      mVertexCount(0),
      mTextIndexBuffer(),
      mTextVertexBuffer()
{
}

DrawableText::~DrawableText()
{
}

void DrawableText::Initialize(
    Dx3d& dx,
    const std::string& inputText,
    Font& font,         // TODO: Make const
    const Size& screenSize,
    const DirectX::SimpleMath::Vector2& position,
    const DirectX::SimpleMath::Color& color)
{
    // Create the vertex and index buffers.
    HRESULT hr = InitializeVertexAndIndexBuffers(dx, inputText.length());
    VerifyDXResult(hr);         // Check for fatal errors.

    // Calculate where we should draw the text, taking into account screen dimensions.
    float drawX = static_cast<float>(((screenSize.width / 2.0f) * -1.0f) + position.x);
    float drawY = static_cast<float>(((screenSize.height / 2.0f) - position.y));

    // Update vertex buffer to correctly display the requested text.
    hr = UpdateVertexBuffer(dx, font, inputText, Vector2(drawX, drawY));
    VerifyDXResult(hr);

    mColor = color;

    SetInitialized();
}

HRESULT DrawableText::InitializeVertexAndIndexBuffers(Dx3d& dx, unsigned int maximumTextLength)
{
    // Initialize values in text object.
    unsigned int textVertexCount = maximumTextLength * RectVertexCount;
    unsigned int textIndexCount = textVertexCount;       // Because tutorial code, that's why.

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

            mMaxTextLength = maximumTextLength;
            mVertexCount = textVertexCount;
            mIndexCount = textIndexCount;
        }
    }

    return hr;
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

HRESULT DrawableText::UpdateVertexBuffer(
    Dx3d& dx,
    Font& font,
    const std::string& inputText,
    const DirectX::SimpleMath::Vector2& position)
{
    // Lock and get access to the vertex buffer. Update each rect in the vertex buffer with the position and UV coords
    // of the character in each slot of the string.
    D3D11_MAPPED_SUBRESOURCE resource;
    HRESULT hr = dx.GetDeviceContext()->Map(mTextVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
    
    if (SUCCEEDED(hr))
    {
        vertex_t * pVerts = reinterpret_cast<vertex_t*>(resource.pData);
        UpdateVertexBufferWithNewText(font, inputText, position, pVerts);

        // Unlock the buffer and return.
        dx.GetDeviceContext()->Unmap(mTextVertexBuffer.Get(), 0);
    }

    return hr;
}

void DrawableText::UpdateVertexBufferWithNewText(
    const Font& font,
    const std::string& inputText,
    const DirectX::SimpleMath::Vector2& position,
    vertex_t * pVertices) const
{
    VerifyNotNull(pVertices);

    float currentX = position.x, currentY = position.y;
    int index = 0;

    for (size_t i = 0; i < inputText.size(); ++i)
    {
        char letter = inputText[i];

        font_char_t characterInfo = font.GetCharInfo(letter);

        float letterSize = static_cast<float>(characterInfo.size);
        float letterLeft = characterInfo.left;
        float letterRight = characterInfo.right;

        // First triangle in quad.
        pVertices[index].position = Vector3(currentX, currentY, 0.0f);  // top left.
        pVertices[index].texture = Vector2(letterLeft, 0.0f);
        index++;

        pVertices[index].position = Vector3((currentX + letterSize), (currentY - CharacterHeight), 0.0f);  // bottom right.
        pVertices[index].texture = Vector2(letterRight, 1.0f);
        index++;

        pVertices[index].position = Vector3(currentX, (currentY - CharacterHeight), 0.0f);  // bottom left.
        pVertices[index].texture = Vector2(letterLeft, 1.0f);
        index++;

        // Second triangle in quad.
        pVertices[index].position = Vector3(currentX, currentY, 0.0f);  // top left.
        pVertices[index].texture = Vector2(letterLeft, 0.0f);
        index++;

        pVertices[index].position = Vector3(currentX + letterSize, currentY, 0.0f);  // top right.
        pVertices[index].texture = Vector2(letterRight, 0.0f);
        index++;

        pVertices[index].position = Vector3((currentX + letterSize), (currentY - CharacterHeight), 0.0f);  // Bottom right.
        pVertices[index].texture = Vector2(letterRight, 1.0f);
        index++;

        // Move to the right to accomodate next character.
        currentX += letterSize + CharacterSpacing;
    }
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
                      font,
                      mIndexCount,
                      worldMatrix,
                      camera.ViewMatrix(),
                      camera.OrthoMatrix(),
                      mColor.ToVector4());          // TODO: Make font shader accept color type.
}