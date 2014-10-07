#pragma once

// TODO: Document, test, etc.
class Size
{
public:
    Size()
        : width(mWidth),
          height(mHeight),
          mWidth(0),
          mHeight(0)
    {
    }

    Size(int width_, int height_)
        : width(mWidth),
          height(mHeight),
          mWidth(width_),
          mHeight(height_)
    {
    }

    Size(unsigned int width_, unsigned int height_)
        : width(mWidth),
          height(mHeight),
          mWidth(width_),
          mHeight(height_)
    {
    }
    
    Size(const Size& size)
        : width(mWidth),
          height(mHeight),
          mWidth(size.mWidth),
          mHeight(size.mHeight)
    {
    }

    Size& operator =(const Size& size)
    {
        mWidth = size.mWidth;
        mHeight = size.mHeight;
        return *this;
    }

    const unsigned int& width;
    const unsigned int& height;

private:
    unsigned int mWidth;
    unsigned int mHeight;
};