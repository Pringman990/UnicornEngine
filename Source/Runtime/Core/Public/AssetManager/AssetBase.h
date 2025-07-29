#pragma once
#include "Core.h"
#include "AssetManager/Internal/IAsset.h"

template<typename T>
class AssetBase : public IAsset
{
public:
	friend class AssetManager;
public:
	enum class AssetState
	{
		Undefined,
		Loading,
		Loaded,
	};
public:
	AssetBase() = default;
	virtual ~AssetBase() = default;

	AssetBase::AssetState GetState() const { return mState; };
	void SetState(AssetState State) { mState = State; };

private:
	AssetState mState = AssetState::Undefined;
};