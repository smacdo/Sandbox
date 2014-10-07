#pragma once

#include "IInitializable.h"
#include "SimpleMath.h"
#include <string>

#include <wrl\wrappers\corewrappers.h>      // ComPtr
#include <wrl\client.h>
#include <memory>

class Font;
class FontShader;
class Dx3d;
class Size;
class Camera;

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Buffer;

// The implemenation of this class is so terrible, it hurts.
class DrawableText : public IInitializable
{
public:
    DrawableText(const std::string& name);
    virtual ~DrawableText() override;

    void Initialize(Dx3d& dx, int maxLength);

    void Update(Dx3d& dx,
                Font& font,
                const std::string& inputText,
                const Size& screenSize,
                const DirectX::SimpleMath::Vector2& position,
                const DirectX::SimpleMath::Color& color);

    void Render(Dx3d& dx,
                Font& font,
                FontShader& fontShader,
                const Camera& camera,
                const DirectX::SimpleMath::Matrix& worldMatrix) const;

    std::string Name() const { return mName; }

protected:
    virtual void OnShutdown() override;

private:
    struct vertex_t     // TODO: do not duplicate, use shared bitmap_vertex_t.
    {
        vertex_t()
            : position(0.0f, 0.0f, 0.0f),
              texture(0.0f, 0.0f)
        {
        }

        DirectX::SimpleMath::Vector3 position;
        DirectX::SimpleMath::Vector2 texture;
    };

private:
    HRESULT CreateTextVertexBuffer(
        Dx3d& dx,
        unsigned int vertexCount,
        ID3D11Buffer **ppVertexBufferOut) const;

    HRESULT CreateTextIndexBuffer(
        Dx3d& dx,
        unsigned int indexCount,
        ID3D11Buffer **ppIndexBufferOut) const;

private:
    std::string mName;          // Name of the drawable text object.
    DirectX::SimpleMath::Color mColor;
    unsigned int mMaxLength;
    unsigned int mIndexCount;
    unsigned int mVertexCount;

    Microsoft::WRL::ComPtr<ID3D11Buffer> mTextIndexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> mTextVertexBuffer;
};

