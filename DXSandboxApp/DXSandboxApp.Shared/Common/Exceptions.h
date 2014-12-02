#pragma once
#include "pch.h"

inline void DXCall(HRESULT hr)
{
    if (FAILED(hr))
    {
        throw Platform::Exception::CreateException(hr, L"DirectX error");
    }
}

inline void Win32Call(unsigned long returnValue)
{
    if (returnValue == 0)
    {
        throw Platform::Exception::CreateException(HRESULT_FROM_WIN32(returnValue), L"Win32 error");
    }
}