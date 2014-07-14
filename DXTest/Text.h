#pragma once

#include "IInitializable.h"
#include "SimpleMath.h"
#include <string>

class Font;
class FontShader;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Buffer;

// This class is so terrible, it hurts.
// TODO: This is not a "text" class, it is a manager of text objects.
// TODO: Change sentence_t to text_t and this class to UiTextManager.
class Text : public IInitializable
{
public:
    Text();
    Text(const Text& text) = delete;
    ~Text();

    Text& operator = (const Text& rhs) = delete;

    void Initialize(ID3D11Device *pDevice,
                    ID3D11DeviceContext *pDeviceContext,
                    int screenWidth,
                    int screenHeight,
                    const DirectX::SimpleMath::Matrix& baseViewMatrix);

    void Render(ID3D11DeviceContext *pDeviceContext,
                const DirectX::SimpleMath::Matrix& worldMatrix,
                const DirectX::SimpleMath::Matrix& orthoMatrix) const;

private:
    struct sentence_t
    {
        ID3D11Buffer * pVertexBuffer;
        ID3D11Buffer * pIndexBuffer;
        int vertexCount, indexCount;
        int maxLength;
        float red, green, blue;
    };

    struct sentence_vertex_t     // TODO: do not duplicate, use shared bitmap_vertex_t.
    {
        DirectX::SimpleMath::Vector3 position;
        DirectX::SimpleMath::Vector2 texture;
    };

private:
    virtual void OnShutdown() override;

    // TODO: These need to be member functions on sentence.
    void InitializeSentence(Text::sentence_t& text, int maxLength, ID3D11Device *pDevice) const;
    void UpdateSentence(Text::sentence_t& text,
                        const std::string& inputText,
                        int x, int y,
                        float r, float g, float b,
                        ID3D11DeviceContext *pDeviceContext) const;
    void ReleaseSentence(Text::sentence_t& text) const;
    void RenderSentence(ID3D11DeviceContext *pDeviceContext,
                        Text::sentence_t& text,
                        const DirectX::SimpleMath::Matrix& worldMatrix,
                        const DirectX::SimpleMath::Matrix& orthoMatrix) const;


private:
    Font * mpFont;
    FontShader * mpFontShader;
    int mScreenWidth;
    int mScreenHeight;
    DirectX::SimpleMath::Matrix mBaseViewMatrix;
    sentence_t * mpSentence1;
    sentence_t * mpSentence2;

};

