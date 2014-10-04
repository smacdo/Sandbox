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
    DrawableText();
    virtual ~DrawableText() override;

    void Initialize(Dx3d& dx, int maxLength);

    void Update(Dx3d& dx,
                Font& font,
                const std::string& inputText,
                const Size& screenSize,
                int x, int y,
                float r, float g, float b);

    void Render(Dx3d& dx,
                Font& font,
                FontShader& fontShader,
                const Camera& camera,
                const DirectX::SimpleMath::Matrix& worldMatrix) const;

protected:
    virtual void OnShutdown() override;

private:
    struct sentence_t
    {
        
       
        
    };

    struct vertex_t     // TODO: do not duplicate, use shared bitmap_vertex_t.
    {
        DirectX::SimpleMath::Vector3 position;
        DirectX::SimpleMath::Vector2 texture;
    };

private:
    unsigned int mMaxLength;
    unsigned int mIndexCount;
    unsigned int mVertexCount;
    float mRed, mGreen, mBlue;

    Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
};

