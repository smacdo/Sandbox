#include "stdafx.h"
#include "TestHelpers.h"
#include "SimpleMath.h"

#include <string>
#include <sstream>

std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString(const DirectX::SimpleMath::Vector4& v)
{
    std::wostringstream ss;
    ss << L"(" << v.x << L", " << v.y << L", " << v.z << L", " << v.w << L")";

    return ss.str();
}

std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString(const DirectX::SimpleMath::Vector3& v)
{
    std::wostringstream ss;
    ss << L"(" << v.x << L", " << v.y << L", " << v.z << L")";

    return ss.str();
}

std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString(const DirectX::SimpleMath::Vector2& v)
{
    std::wostringstream ss;
    ss << L"(" << v.x << L", " << v.y << L")";

    return ss.str();
}