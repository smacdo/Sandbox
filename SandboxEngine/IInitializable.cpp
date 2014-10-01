#include "stdafx.h"
#include "IInitializable.h"

IInitializable::IInitializable()
    : mInitialized(false)
{
}

IInitializable::~IInitializable()
{
    Shutdown();
}

void IInitializable::Shutdown()
{
    if (mInitialized)
    {
        OnShutdown();
        mInitialized = false;
    }
}

bool IInitializable::IsInitialized() const
{
    return mInitialized;
}

void IInitializable::SetInitialized()
{
    if (!mInitialized)
    {
        OnInitialize();
        mInitialized = true;
    }
}

void IInitializable::OnShutdown()
{
    // default implementation does nothing.
}

void IInitializable::OnInitialize()
{
    // default implementation does nothing.
}
