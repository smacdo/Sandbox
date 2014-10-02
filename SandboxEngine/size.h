#pragma once

// TODO: Document, test, etc.
class Size
{
public:
    Size(int width, int height)
        : mWidth(width),
          mHeight(height)
    {
    }

    Size(unsigned int width, unsigned int height)
        : mWidth(width),
          mHeight(height)
    {
    }

    unsigned int Width() const { return mWidth; }
    unsigned int Height() const { return mHeight; }

private:
    unsigned int mWidth;
    unsigned int mHeight;
};