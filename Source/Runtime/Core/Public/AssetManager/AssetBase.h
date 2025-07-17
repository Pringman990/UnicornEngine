#pragma once
#include "Core.h"
#include "AssetManager/Internal/IAsset.h"

template<typename T>
concept HasStagingData = requires { typename T::StagingData; };

template<typename T>
concept HasFreeStagingData = requires(T t) { t.FreeStagingData(); };

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
		GPU_Uploaded
	};
public:
	AssetBase() = default;
	virtual ~AssetBase() = default;

	AssetBase::AssetState GetState() const { return mState; };
	void SetState(AssetState State) { mState = State; };
private:
	/// <summary>
	/// Should never be called.
	/// Function to makes sure some functions have been implmented
	/// </summary>
	void Free()
	{
		if constexpr (HasStagingData<T>)
		{
			static_assert(
				HasFreeStagingData<T>,
				"[AssetBase Error] Type T has StagingData but is missing FreeStagingData() implementation, please implement void FreeStagingData();"
				);

			static_cast<T*>(this)->FreeStagingData();
		}
	};


private:
	AssetState mState = AssetState::Undefined;
};