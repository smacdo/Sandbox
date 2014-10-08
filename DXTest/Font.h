#pragma once

#include <string>
#include <vector>
#include <memory>
#include "IInitializable.h"

struct ID3D11Device;
struct ID3D11ShaderResourceView;
class Texture;

struct font_char_t      // TODO: Document this struct.
{
    float left, right;      // these are texture UV coordinates for the char texture.
    int size;        // pixel width
};

class Font : public IInitializable
{
public:
    Font();
    Font(const Font& font) = delete;

    Font& operator =(const Font& rhs) = delete;

    void Initialize(ID3D11Device * pDevice, const std::wstring& layoutFile, const std::wstring& textureFile);
    const Texture& GetTexture() const;

    font_char_t GetCharInfo(char c) const;

protected:
    virtual void OnShutdown() override;

private:
    std::vector<font_char_t> LoadFontLayout(const std::wstring& layoutFile) const;

private:
    std::vector<font_char_t> mCharInfo;
    std::unique_ptr<Texture> mTexture;
};

