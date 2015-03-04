#include "pch.h"
#include "Utility.h"

#include <string>
#include <locale>
#include <memory>
#include <codecvt>
#include <windows.h>

using namespace DXSandboxApp;
using namespace Microsoft::WRL;

inline void DXSandboxApp::DebugPrint(const std::wstring& text)
{
	std::wstring output = text + L"\n";
	OutputDebugStringW(output.c_str());
}

void DXSandboxApp::SetDebugName(
	_In_ IUnknown * pResource,
	_In_ const std::wstring& name)
{
#if defined (_DEBUG)		// Only assign debug names in debug builds.
	if (pResource == nullptr)
	{
		return;
	}

	const int NameBufferSize = 1024;
	char nameBuffer[NameBufferSize];

	int length = WideCharToMultiByte(
		CP_ACP,
		0,
		name.c_str(),
		-1,
		nameBuffer,
		NameBufferSize,
		nullptr,
		nullptr);

	if (length > 0)
	{
		ComPtr<ID3D11DeviceChild> deviceChild;
		HRESULT hr = pResource->QueryInterface(IID_PPV_ARGS(&deviceChild));

		if (SUCCEEDED(hr))
		{
			// TODO: Maybe we should record if fail to set a name...
			hr = deviceChild->SetPrivateData(
				WKPDID_D3DDebugObjectName,
				NameBufferSize - 1,
				nameBuffer);
		}
		else
		{
			return;			// WE FAILED.
		}
	}
#endif
}

std::wstring DXSandboxApp::ToWideString(const std::string& string)
{
	typedef std::codecvt_utf8<wchar_t> convert_type;
	std::wstring_convert<convert_type, wchar_t> converter;

	return converter.from_bytes(string);
}

std::string DXSandboxApp::ToUtf8String(const std::wstring& string)
{
	typedef std::codecvt_utf8<wchar_t> convert_type;
	std::wstring_convert<convert_type, wchar_t> converter;

	return converter.to_bytes(string);
}