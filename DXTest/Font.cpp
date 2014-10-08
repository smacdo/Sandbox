#include "Font.h"
#include "Texture.h"
#include "DXSandbox.h"
#include "DXTestException.h"
#include "Utils.h"
#include "SimpleMath.h"

#include <d3d11.h>

#include <fstream>
#include <vector>
#include <memory>

const int FONT_CHAR_COUNT = 95;

using namespace DirectX::SimpleMath;

struct font_vertex_t        // TODO: Use the Texture::vertex_t data type instead of duplication.
{
    Vector3 position;
    Vector2 texture;
};

Font::Font()
: IInitializable(),
  mCharInfo(),
  mTexture()
{
}

void Font::Initialize(ID3D11Device *pDevice, const std::wstring& layoutFile, const std::wstring& textureFile)
{
    if (IsInitialized()) { return; }
    VerifyNotNull(pDevice);

    mCharInfo = LoadFontLayout(layoutFile);
    
    mTexture.reset(new Texture());
    mTexture->InitializeFromFile(pDevice, textureFile);
}

std::vector<font_char_t> Font::LoadFontLayout(const std::wstring& layoutFile) const
{
    // Font format layout:
    // [Character ascii value] [Character] [Left tu coord] [Right tu cord] [Pixel width]
    std::vector<font_char_t> fontInfo(FONT_CHAR_COUNT);

    // Read the font layout values from the text file.
    std::fstream layoutStream(layoutFile.c_str());

    if (layoutStream.fail())
    {
        throw FileLoadException(layoutFile);
    }

    for (int i = 0; i < FONT_CHAR_COUNT; ++i)
    {
        // Read through data in file we do not need.
        int asciiValue;
        char charValue;

        layoutStream >> asciiValue;
        layoutStream >> charValue;

        // Now read values.
        layoutStream >> fontInfo[i].left;
        layoutStream >> fontInfo[i].right;
        layoutStream >> fontInfo[i].size;
    }

    layoutStream.close();
    return fontInfo;
}

void Font::OnShutdown()
{
    mCharInfo.resize(0);
}

const Texture& Font::GetTexture() const
{
    return *mTexture.get();
}

font_char_t Font::GetCharInfo(char c) const
{
    const int NumberOfCharactersBeforeFirstLetter = 32;
    Assert(c >= NumberOfCharactersBeforeFirstLetter);

    unsigned int letter = static_cast<int>(c) - NumberOfCharactersBeforeFirstLetter;
    Assert(letter >= 0u && letter < mCharInfo.size());

    return mCharInfo[letter];
}