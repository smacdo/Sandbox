#pragma once

#include <string>

namespace DXSandboxApp
{
	// Returns a size suitable for creating constant buffers by rounding the requested size up to the next multiple of
	// sixteen.
	inline size_t RoundToNextConstantBufferSize(size_t unroundedSize)
	{
		return ((unroundedSize + 15) / 16) * 16;
	}

	inline void DebugPrint(const std::wstring& text);
}

