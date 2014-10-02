#include "stdafx.h"
#include "Frustum.h"
#include "Range.h"
#include "SimpleMath.h"

using namespace DirectX::SimpleMath;

Frustum::Frustum()
    : mPlanes()
{
    Clear();
}

bool Frustum::operator ==(const Frustum& rhs) const
{
    for (auto i : MakeRange<0, FrustumPlaneCount>())
    {
        if (mPlanes[i] != rhs.mPlanes[i])
        {
            return false;
        }
    }

    return true;
}

bool Frustum::operator !=(const Frustum& rhs) const
{
    for (auto i : MakeRange<0, FrustumPlaneCount>())
    {
        if (mPlanes[i] == rhs.mPlanes[i])
        {
            return false;
        }
    }

    return true;
}

void Frustum::Update(float screenDepth, const Matrix& inProjectionMatrix, const Matrix& viewMatrix)
{
    // TODO: This whole thing is a mess. Find some reference article that explains the math and workings of
    // this code to make more sense of it all...

    // Calculate the minimum Z distance in the frustum.
    Matrix projectionMatrix = inProjectionMatrix;

    float zMinimum = -projectionMatrix._43 / projectionMatrix._33;
    float r = screenDepth / (screenDepth - zMinimum);

    projectionMatrix._33 = r;
    projectionMatrix._43 = -r * zMinimum;

    // Create the frustum matrix from the view matrix, and updated projection matrix.
    Matrix fMatrix = viewMatrix * projectionMatrix;

    // Near frustum plane.
    mPlanes[0].x = fMatrix._14 + fMatrix._13;
    mPlanes[0].y = fMatrix._24 + fMatrix._23;
    mPlanes[0].z = fMatrix._34 + fMatrix._33;
    mPlanes[0].w = fMatrix._44 + fMatrix._43;

    mPlanes[0].Normalize();

    // Far frustum plane.
    mPlanes[1].x = fMatrix._14 - fMatrix._13;
    mPlanes[1].y = fMatrix._24 - fMatrix._23;
    mPlanes[1].z = fMatrix._34 - fMatrix._33;
    mPlanes[1].w = fMatrix._44 - fMatrix._43;

    mPlanes[1].Normalize();

    // Left frustum plane.
    mPlanes[2].x = fMatrix._14 + fMatrix._11;
    mPlanes[2].y = fMatrix._24 + fMatrix._21;
    mPlanes[2].z = fMatrix._34 + fMatrix._31;
    mPlanes[2].w = fMatrix._44 + fMatrix._41;

    mPlanes[2].Normalize();

    // Right frustum plane.
    mPlanes[3].x = fMatrix._14 - fMatrix._11;
    mPlanes[3].y = fMatrix._24 - fMatrix._21;
    mPlanes[3].z = fMatrix._34 - fMatrix._31;
    mPlanes[3].w = fMatrix._44 - fMatrix._41;

    mPlanes[3].Normalize();

    // Top frustum plane.
    mPlanes[4].x = fMatrix._14 - fMatrix._12;
    mPlanes[4].y = fMatrix._24 - fMatrix._22;
    mPlanes[4].z = fMatrix._34 - fMatrix._32;
    mPlanes[4].w = fMatrix._44 - fMatrix._42;

    mPlanes[4].Normalize();

    // Bottom frustum plane.
    mPlanes[5].x = fMatrix._14 + fMatrix._12;
    mPlanes[5].y = fMatrix._24 + fMatrix._22;
    mPlanes[5].z = fMatrix._34 + fMatrix._32;
    mPlanes[5].w = fMatrix._44 + fMatrix._42;

    mPlanes[5].Normalize();
}

// Check if the given point is inside the viewing frustum.
bool Frustum::CheckPoint(const Vector3& point) const
{
    // Check if the point is inside the viewing frustum's six planes.
    for (auto i : MakeRange<0, FrustumPlaneCount>())
    {
        if (mPlanes[i].DotCoordinate(point) < 0.0f)
        {
            return false;
        }
    }

    return true;
}

bool Frustum::CheckCube(const Vector3& center, float radius) const
{
    // Check that any of the cube's eight corner corner points are inside the viewing frustum.
    Vector3 a(center.x - radius, center.y - radius, center.z - radius);
    Vector3 b(center.x + radius, center.y - radius, center.z - radius);
    Vector3 c(center.x - radius, center.y + radius, center.z - radius);
    Vector3 d(center.x + radius, center.y + radius, center.z - radius);
    Vector3 e(center.x - radius, center.y - radius, center.z + radius);
    Vector3 f(center.x + radius, center.y - radius, center.z + radius);
    Vector3 g(center.x - radius, center.y + radius, center.z + radius);
    Vector3 h(center.x + radius, center.y + radius, center.z + radius);

    for (auto i : MakeRange<0, FrustumPlaneCount>())
    {
        if (mPlanes[i].DotCoordinate(a) < 0.0f && mPlanes[i].DotCoordinate(b) < 0.0f &&
            mPlanes[i].DotCoordinate(c) < 0.0f && mPlanes[i].DotCoordinate(d) < 0.0f &&
            mPlanes[i].DotCoordinate(e) < 0.0f && mPlanes[i].DotCoordinate(f) < 0.0f &&
            mPlanes[i].DotCoordinate(g) < 0.0f && mPlanes[i].DotCoordinate(h) < 0.0f )
        {
            return false;
        }
    }

    return true;
}

bool Frustum::CheckSphere(const Vector3& center, float radius) const
{
    // Check if the outer extent of the sphere (center + radius) is inside all six planes of the viewing frustum.
    for (auto i : MakeRange<0, FrustumPlaneCount>())
    {
        if (mPlanes[i].DotCoordinate(center) < -radius)
        {
            return false;
        }
    }

    return true;
}

bool Frustum::CheckRectangle(const Vector3& center, const Vector3& size) const
{
    Vector3 a(center.x - size.x, center.y - size.y, center.z - size.z);
    Vector3 b(center.x + size.x, center.y - size.y, center.z - size.z);
    Vector3 c(center.x - size.x, center.y + size.y, center.z - size.z);
    Vector3 d(center.x - size.x, center.y - size.y, center.z + size.z);
    Vector3 e(center.x + size.x, center.y + size.y, center.z - size.z);
    Vector3 f(center.x + size.x, center.y - size.y, center.z + size.z);
    Vector3 g(center.x - size.x, center.y + size.y, center.z + size.z);
    Vector3 h(center.x + size.x, center.y + size.y, center.z + size.z);

    for (auto i : MakeRange<0, FrustumPlaneCount>())
    {
        if (mPlanes[i].DotCoordinate(a) < 0.0f && mPlanes[i].DotCoordinate(b) < 0.0f &&
            mPlanes[i].DotCoordinate(c) < 0.0f && mPlanes[i].DotCoordinate(d) < 0.0f &&
            mPlanes[i].DotCoordinate(e) < 0.0f && mPlanes[i].DotCoordinate(f) < 0.0f &&
            mPlanes[i].DotCoordinate(g) < 0.0f && mPlanes[i].DotCoordinate(h))
        {
            return false;
        }
    }

    return true;
}

void Frustum::Clear()
{
    for (auto i : MakeRange<0, FrustumPlaneCount>())
    {
        mPlanes[i] = Plane(0.0f, 0.0f, 0.0f, 0.0f);
    }
}
