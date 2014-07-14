#pragma once

#include <string>
#include "IInitializable.h"

struct ID3D11Device;
struct ID3D11ShaderResourceView;
class Texture;

class Font : public IInitializable
{
public:
    Font();
    Font(const Font& font) = delete;
    ~Font();

    Font& operator =(const Font& rhs) = delete;

    void Initialize(ID3D11Device * pDevice, const std::string& layoutFile, const std::string& textureFile);

    // TODO: Return the texture object, not the contained shader view.
    ID3D11ShaderResourceView * GetTexture() const;

    // Seriously, WTF. TODO: Fix this abomination.
    void BuildVertexArray(void*, const std::string&, float, float) const;

protected:
    virtual void OnShutdown() override;

private:
    void LoadFontLayout(const std::string& layoutFile);
    void LoadTexture(ID3D11Device *pDevice, const std::string& textureFile);

private:
    struct font_char_t
    {
        float left, right;
        int size;        // pixel width
    };

private:
    font_char_t * mpCharInfo;
    Texture * mpTexture;
};

