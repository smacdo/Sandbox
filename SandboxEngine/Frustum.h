#pragma once
#include "SimpleMath.h"
#include <array>

// TODO: Convert Check* to use structures representing the primitive, or at least wrap up the vectors.
class Frustum
{
public:
    Frustum();

    // Comparison.
    bool operator == (const Frustum& rhs) const;
    bool operator != (const Frustum& rhs) const;

    // Operations.
    bool CheckPoint(const DirectX::SimpleMath::Vector3& point) const;
    bool CheckCube(const DirectX::SimpleMath::Vector3& center, float radius) const;
    bool CheckSphere(const DirectX::SimpleMath::Vector3& center, float radius) const;
    bool CheckRectangle(const DirectX::SimpleMath::Vector3& center, const DirectX::SimpleMath::Vector3& size) const;

    void Update(float screenDepth,
                const DirectX::SimpleMath::Matrix& projectionMatrix,
                const DirectX::SimpleMath::Matrix& viewMatrix);
    void Clear();

private:
    static const int FrustumPlaneCount = 6;
    std::array<DirectX::SimpleMath::Plane, FrustumPlaneCount> mPlanes;
};

