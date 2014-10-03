#pragma once

// TODO: Document, test, etc.
class Size
{
public:
    Size()
        : width(0),
          height(0)
    {
    }

    Size(int width_, int height_)
        : width(width_),
          height(height_)
    {
    }

    Size(unsigned int width_, unsigned int height_)
        : width(width_),
          height(height_)
    {
    }

    const unsigned int width;
    const unsigned int height;
};