#include "Assets/AssetBase.h"

AssetBase::AssetBase(UniqueID128 UUID)
	:
	mSourcePath("INVALID"),
	mMetaPath("INVALID"),
	mUUID(UUID)
{
}

AssetBase::~AssetBase()
{
}
