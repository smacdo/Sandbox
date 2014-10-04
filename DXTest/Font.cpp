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

Font::~Font()
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

std::vector<Font::font_char_t> Font::LoadFontLayout(const std::wstring& layoutFile) const
{
    // Font format layout:
    // [Character ascii value] [Character] [Left tu coord] [Right tu cord] [Pixel width]
    std::vector<Font::font_char_t> fontInfo(FONT_CHAR_COUNT);

    // Read the font layout values from the text file.
    std::fstream layoutStream(layoutFile.c_str());
    char temp;

    if (layoutStream.fail())
    {
        throw FileLoadException(layoutFile);
    }

    for (int i = 0; i < FONT_CHAR_COUNT; ++i)
    {
        // Read through data in file we do not need.
        layoutStream.get(temp);
        while (temp != ' ') { layoutStream.get(temp); }

        layoutStream.get(temp);
        while (temp != ' ') { layoutStream.get(temp); }

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
    mTexture.reset();
}

// BuildVertexArray is called by the Text class to build vertex buffers out of text sentences. This way each sentence
// that needs to be drawn has its own vertex buffer for rendering. The x,y coordinates are the screen coordinates of
// where to draw the sentence.
//   TODO: Commendable, but terrible implementation. Figure out how to make this abomdination better and much better
//         OOP design. Especially the WTFy void*.
void Font::BuildVertexArray(void* pVertices, const std::string& text, float startX, float startY) const
{
    font_vertex_t * pVerts = reinterpret_cast<font_vertex_t*>(pVertices);        // oh god.
    float currentX = startX, currentY = startY;
    int index = 0;

    for (size_t i = 0; i < text.size(); ++i)
    {
        int letter = ((int)text[i]) - 32;       // TODO: magic numbers, bad.

        // If the letter is a space then just move over three pixels. TODO: Make three pixels not magic number.
        if (letter == 0)
        {
            currentX += 3.0f;
        }
        else
        {
            // TODO: Way too many magic numbers in here.
            float letterSize = static_cast<float>(mCharInfo[letter].size);
            float letterLeft = mCharInfo[letter].left;
            float letterRight = mCharInfo[letter].right;

            // First triangle in quad.
            pVerts[index].position = Vector3(currentX, currentY, 0.0f);  // top left.
            pVerts[index].texture = Vector2(letterLeft, 0.0f);
            index++;

            pVerts[index].position = Vector3((currentX + letterSize), (currentY - 16), 0.0f);  // bottom right.
            pVerts[index].texture = Vector2(letterRight, 1.0f);
            index++;

            pVerts[index].position = Vector3(currentX, (currentY - 16), 0.0f);  // bottom left.
            pVerts[index].texture = Vector2(letterLeft, 1.0f);
            index++;

            // Second triangle in quad.
            pVerts[index].position = Vector3(currentX, currentY, 0.0f);  // top left.
            pVerts[index].texture = Vector2(letterLeft, 0.0f);
            index++;

            pVerts[index].position = Vector3(currentX + letterSize, currentY, 0.0f);  // top right.
            pVerts[index].texture = Vector2(letterRight, 0.0f);
            index++;

            pVerts[index].position = Vector3((currentX + letterSize), (currentY - 16), 0.0f);  // Bottom right.
            pVerts[index].texture = Vector2(letterRight, 1.0f);
            index++;

            // Move to the right to accomodate next character.
            currentX += letterSize + 1.0f;      // TODO: Magic number
        }
    }
}

ID3D11ShaderResourceView * Font::GetTexture()
{
    return mTexture->GetTexture();
}

ID3D11ShaderResourceView * Font::GetTexture() const  // TODO: Reconcile
{
    return mTexture->GetTexture();
}