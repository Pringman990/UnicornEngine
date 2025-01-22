#include "pch.h"
#include "RenderTarget/RenderTarget.h"

#include <RenderTarget/RenderTargetResourceManager.h>

RenderTarget::RenderTarget()
    : 
    mTexture(nullptr),
    mIsDepthTesting(true)
{
}

RenderTarget::~RenderTarget()
{
}

void RenderTarget::Destroy()
{
    RenderTargetResourceManager* manager = AssetRegistry::GetInstance()->GetManager<RenderTargetResourceManager>();
    manager->DestroyRenderTarget(this);
}

bool RenderTarget::IsDepthTesting()
{
    return mIsDepthTesting;
}

Texture* RenderTarget::GetTexture()
{
    return mTexture;
}


