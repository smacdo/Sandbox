#pragma once

#include <string>

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

template<typename T>
inline void CheckNotNull(T* ptr)
{
	if (ptr == nullptr)
	{
		throw ref new Platform::NullReferenceException();
	}
}

namespace DXSandboxApp
{
	// Returns a size suitable for creating constant buffers by rounding the requested size up to the next multiple of
	// sixteen.
	inline size_t RoundToNextConstantBufferSize(size_t unroundedSize)
	{
		return ((unroundedSize + 15) / 16) * 16;
	}

	inline void DebugPrint(const std::wstring& text);

	std::wstring ToWideString(const std::string& string);
	std::string ToUtf8String(const std::wstring& string);

	void SetDebugName(
		_In_ IUnknown * pResource,
		_In_ const std::wstring& name);
}

