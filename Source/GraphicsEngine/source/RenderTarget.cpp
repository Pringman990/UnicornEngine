#include "GraphicsPch.h"
#include "RenderTarget.h"

RenderTarget::RenderTarget()
{
}

RenderTarget::~RenderTarget()
{
}

bool RenderTarget::Create(Vector2 aSize)
{
    mSize = aSize;
    return false;
}
