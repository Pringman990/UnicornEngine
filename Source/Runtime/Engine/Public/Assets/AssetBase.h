#pragma once
#include <Core.h>
#include "EngineDefines.h"
#include "ResourceManagment/ResourceHandle.h"

/**
* Base class for all assets. 
*/
class AssetBase
{
public:
	ENGINE_API AssetBase(UniqueID128 UUID);
	ENGINE_API virtual ~AssetBase();

	void SetSourcePath(const String& Path) { mSourcePath = Path; };
	void SetMetaPath(const String& Path) { mMetaPath = Path; };
	
	/**
	* @warning
	* Make sure the handle passed is a GPUTexture handle.
	*/
	void SetCachedThumbnailHandle(GenericHandle Handle) { mCachedThumbnailHandle = Handle; };

	const String& GetSourcePath() const { return mSourcePath; };
	const String& GetMetaPath() const { return mMetaPath; };
	const UniqueID128& GetUUID() const { return mUUID; };
	GenericHandle GetCachedTumbnailHandle() const { return mCachedThumbnailHandle; };

private:
	String mSourcePath;
	String mMetaPath;
	UniqueID128 mUUID;

	/**
	* @note
	* Will be converted to GPUTexture handle.
	*/
	GenericHandle mCachedThumbnailHandle;
};