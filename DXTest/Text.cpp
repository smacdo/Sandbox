#include "Text.h"
#include "Font.h"
#include "FontShader.h"
#include "DXSandbox.h"
#include "Dx3d.h"

#include "SimpleMath.h"
#include <d3d11.h>

using namespace DirectX::SimpleMath;



Text::Text()
: IInitializable(),
  mpFont(nullptr),
  mpFontShader(nullptr),
  mScreenWidth(0),
  mScreenHeight(0),
  mBaseViewMatrix(),
  mpSentence1(nullptr),
  mpSentence2(nullptr)
{
}

Text::~Text()
{
}

void Text::Initialize(Dx3d& dx,
                      int screenWidth,
                      int screenHeight,
                      const DirectX::SimpleMath::Matrix& baseViewMatrix)
{
    // Store screen width and height.
    mScreenWidth = screenWidth;
    mScreenHeight = screenHeight;
    mBaseViewMatrix = baseViewMatrix;

    // Create font object and shader.
    mpFont = new Font();
    mpFont->Initialize(dx.GetDevice(), L".\\Fonts\\rastertek.txt", L".\\Fonts\\rastertek.dds");

    mpFontShader = new FontShader();
    mpFontShader->Initialize(dx);

    // Initialize sentences.
    mpSentence1 = new Text::sentence_t;
    InitializeSentence(*mpSentence1, 16, dx);
    UpdateSentence(*mpSentence1, "Hello", 100, 100, 1.0f, 1.0f, 1.0f, dx);

    mpSentence2 = new Text::sentence_t;
    InitializeSentence(*mpSentence2, 16, dx);
    UpdateSentence(*mpSentence2, "World", 100, 200, 1.0f, 1.0f, 0.0f, dx);
}

void Text::OnShutdown()
{
    ReleaseSentence(*mpSentence1);
    ReleaseSentence(*mpSentence2);

    SafeDelete(mpSentence1);
    SafeDelete(mpSentence2);
    SafeDelete(mpFontShader);
    SafeDelete(mpFont);
}

void Text::Render(Dx3d& dx,
                  const DirectX::SimpleMath::Matrix& worldMatrix,
                  const DirectX::SimpleMath::Matrix& orthoMatrix) const
{    
    RenderSentence(dx, *mpSentence1, worldMatrix, orthoMatrix);
    RenderSentence(dx, *mpSentence2, worldMatrix, orthoMatrix);
}

void Text::InitializeSentence(Text::sentence_t& text, int maxLength, Dx3d& dx) const
{
    // Initialize values in text object.
    text.maxLength = maxLength;
    text.vertexCount = 6 * maxLength;       // TODO: Magic number.
    text.indexCount = text.vertexCount;

    // Allocate temporary software vertex and index arrays.
    sentence_vertex_t * pVertices = new sentence_vertex_t[text.vertexCount];
    unsigned long * pIndices = new unsigned long[text.indexCount];

    // Initialize vertex array to zero, and the index array to point to each vertex in order.
    memset(pVertices, 0, sizeof(sentence_vertex_t)* text.vertexCount);

    for (int i = 0; i < text.indexCount; ++i)
    {
        pIndices[i] = i;
    }

    // Create a dynamic vertex buffer to store the text vertices. This allows us to update the contentents of the
    // text at any time.
    D3D11_BUFFER_DESC vbd;

    vbd.Usage = D3D11_USAGE_DYNAMIC;
    vbd.ByteWidth = sizeof(sentence_vertex_t)* text.vertexCount;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vbd.MiscFlags = 0;
    vbd.StructureByteStride = 0;
    
    D3D11_SUBRESOURCE_DATA vertexData;

    vertexData.pSysMem = pVertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    HRESULT result = dx.GetDevice()->CreateBuffer(&vbd, &vertexData, &(text.pVertexBuffer));

    VerifyDXResult(result);
    VerifyNotNull(text.pVertexBuffer);

    // Create a static index buffer. It is static because we do not need to reorder the vertices of the text mesh.
    D3D11_BUFFER_DESC ibd;

    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = sizeof(unsigned long)* text.indexCount;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    ibd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA indexData;

    indexData.pSysMem = pIndices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    result = dx.GetDevice()->CreateBuffer(&ibd, &indexData, &(text.pIndexBuffer));

    VerifyDXResult(result);
    VerifyNotNull(text.pIndexBuffer);

    // Release temporary arrays now that we have created the hardware buffers.
    SafeDeleteArray(pVertices);
    SafeDeleteArray(pIndices);
}

void Text::UpdateSentence(Text::sentence_t& text,
                          const std::string& inputText,
                          int x, int y,
                          float r, float g, float b,
                          Dx3d& dx) const
{
    Verify(static_cast<unsigned int>(text.maxLength) > inputText.size());

    // Store the requested color.
    text.red = r;
    text.green = g;
    text.blue = b;

    // Create a temporary vertex array.
    //  TODO: Just use the mapped hardware array.
    sentence_vertex_t * pVertices = new sentence_vertex_t[text.vertexCount];
    memset(pVertices, 0, sizeof(sentence_vertex_t)* text.vertexCount);

    // Calculate the x and y pixel position on screen to draw at.
    float drawX = static_cast<float>(((mScreenWidth / 2.0f) * -1.0f) + x);
    float drawY = static_cast<float>(((mScreenHeight / 2.0f) - y));

    // Use the font class to build the vertex array.
    mpFont->BuildVertexArray(reinterpret_cast<void*>(pVertices), inputText, drawX, drawY);

    // Lock the vertex buffer and copy the new vertex array into that buffer.
    D3D11_MAPPED_SUBRESOURCE resource;
    HRESULT result = dx.GetDeviceContext()->Map(text.pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

    VerifyDXResult(result);
    VerifyNotNull(resource.pData);

    sentence_vertex_t * pVerts = reinterpret_cast<sentence_vertex_t*>(resource.pData);
    memcpy(pVerts, reinterpret_cast<void*>(pVertices), sizeof(sentence_vertex_t)* text.vertexCount);

    dx.GetDeviceContext()->Unmap(text.pVertexBuffer, 0);

    // Clean up no longer needed vertex array.
    SafeDeleteArray(pVertices);
}

void Text::ReleaseSentence(Text::sentence_t& text) const
{
    SafeRelease(text.pVertexBuffer);
    SafeRelease(text.pIndexBuffer);
}

void Text::RenderSentence(Dx3d& dx,
                          Text::sentence_t& text,
                          const DirectX::SimpleMath::Matrix& worldMatrix,
                          const DirectX::SimpleMath::Matrix& orthoMatrix) const
{
    unsigned int stride = sizeof(sentence_vertex_t);
    unsigned int offset = 0;

    // Activate the vertex and index buffers for rendering.
    dx.GetDeviceContext()->IASetVertexBuffers(0, 1, &text.pVertexBuffer, &stride, &offset);
    dx.GetDeviceContext()->IASetIndexBuffer(text.pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    dx.GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Render the text by way of shader. TODO: Don't do it like this.
    Vector4 pixelColor(text.red, text.green, text.blue, 1.0f);

    mpFontShader->Render(dx,
                         text.indexCount,
                         worldMatrix,
                         mBaseViewMatrix,
                         orthoMatrix,
                         mpFont->GetTexture(),
                         pixelColor);
}