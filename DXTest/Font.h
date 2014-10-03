#pragma once

#include <string>
#include <vector>
#include <memory>
#include "IInitializable.h"

struct ID3D11Device;
struct ID3D11ShaderResourceView;
class Texture;

class Font : public IInitializable
{
public:
    Font();
    Font(const Font& font) = delete;
    virtual ~Font() override;

    Font& operator =(const Font& rhs) = delete;

    void Initialize(ID3D11Device * pDevice, const std::wstring& layoutFile, const std::wstring& textureFile);

    ID3D11ShaderResourceView * GetTexture();

    // Seriously, WTF. TODO: Fix this abomination.
    void BuildVertexArray(void*, const std::string&, float, float) const;

protected:
    virtual void OnShutdown() override;

private:
    struct font_char_t
    {
        float left, right;
        int size;        // pixel width
    };

private:
    std::vector<font_char_t> LoadFontLayout(const std::wstring& layoutFile) const;

private:
    std::vector<font_char_t> mCharInfo;
    std::unique_ptr<Texture> mTexture;
};

