#pragma once

template<typename _AssetType>
class IAssetManager
{
public:
	virtual ~IAssetManager() = default;

	
	virtual _AssetType* LoadAsset() = 0;

private:

};