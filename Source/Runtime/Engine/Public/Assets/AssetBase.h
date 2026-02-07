#pragma once
#include <Core.h>
#include "EngineDefines.h"
#include "ResourceManagment/ResourceHandle.h"

enum class AssetLoadStage
{
	Registered, //The uuid has been registered but the asset data has not yet been loaded.
	Loaded
};

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
	void SetName(const String& Name) { mName = Name; };
	void SetType(const String& Type) { mType = Type; };
	void SetLoadStage(AssetLoadStage Stage) { mLoadStage = Stage; };
	
	/**
	* @warning
	* Make sure the handle passed is a GPUTexture handle.
	*/
	void SetCachedThumbnailHandle(GenericHandle Handle) { mCachedThumbnailHandle = Handle; };

	const String& GetSourcePath() const { return mSourcePath; };
	const String& GetMetaPath() const { return mMetaPath; };
	const String& GetName() const { return mName; };
	const String& GetType() const { return mType; };
	const UniqueID128& GetUUID() const { return mUUID; };
	AssetLoadStage GetLoadStage() const { return mLoadStage; };
	GenericHandle GetCachedTumbnailHandle() const { return mCachedThumbnailHandle; };

private:
	friend class AssetRegistry;

	String mSourcePath;
	String mMetaPath;
	String mName;
	String mType;
	UniqueID128 mUUID;
	AssetLoadStage mLoadStage;

	/**
	* @note
	* Will be converted to GPUTexture handle.
	*/
	GenericHandle mCachedThumbnailHandle;
};