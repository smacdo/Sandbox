#pragma once
#include "SimpleMath.h"
#include <string>

namespace Microsoft
{
    namespace VisualStudio
    {
        namespace CppUnitTestFramework
        {
            std::wstring ToString(const DirectX::SimpleMath::Vector4& v);
            std::wstring ToString(const DirectX::SimpleMath::Vector3& v);
            std::wstring ToString(const DirectX::SimpleMath::Vector2& v);
        }
    }
}