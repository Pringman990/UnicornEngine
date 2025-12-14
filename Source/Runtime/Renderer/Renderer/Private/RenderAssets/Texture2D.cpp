#include "RenderAssets/Texture2D.h"

Texture2D::Texture2D()
	:
	AssetBase({})
{
}

Texture2D::Texture2D(UniqueID128 UUID) 
	:
	AssetBase(UUID)
{
}

Texture2D::~Texture2D()
{
}
