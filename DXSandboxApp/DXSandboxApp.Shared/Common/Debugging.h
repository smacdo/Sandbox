#pragma once
#include <cassert>

template<typename T>
inline void CheckNotNull(T* ptr)
{
    if (ptr == nullptr)
    {
        throw ref new Platform::NullReferenceException();
    }
}