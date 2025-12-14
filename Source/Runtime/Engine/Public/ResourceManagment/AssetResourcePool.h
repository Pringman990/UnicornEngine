#pragma once
#include <Core.h>

#include "Logger/Logger.h"
#include "Assets/AssetRef.h"

#include <utility> //std::move

/**
* Resource pool where all assets are on the heap and private using ownedptr.
*/
template<typename T>
class AssetResourcePool
{
public:
	struct Entry
	{
		OwnedPtr<T> asset;
		uint32 generation;
	};

	AssetResourcePool() = default;
	~AssetResourcePool() = default;

	const AssetRefHandle Allocate()
	{
		uint32 index = 0;
		if (!mFreeIndices.empty())
		{
			index = mFreeIndices.front();
			mFreeIndices.pop();

			mEntries[index].asset = MakeOwned<T>();
			mEntries[index].generation++;
		}
		else
		{
			index = static_cast<uint32>(mEntries.size());
			mEntries.push_back({ MakeOwned<T>(), 1 });
		}

		return AssetRefHandle{index, mEntries[index].generation};
	}

	uint32 GetCount() const
	{
		return static_cast<uint32>(mEntries.size());
	}

	//template<typename Asset, typename args>
	//const AssetRefHandle Allocate(args Args...)
	//{
	//	static_assert(std::is_base_of<AssetBase, Asset>::value, "Asset must be derived from AssetBase");

	//	uint32 index = 0;
	//	if (!mFreeIndices.empty())
	//	{
	//		index = mFreeIndices.front();
	//		mFreeIndices.pop();

	//		mEntries[index].asset = MakeOwned<Asset>(Args);
	//		mEntries[index].generation++;
	//	}
	//	else
	//	{
	//		index = static_cast<uint32>(mEntries.size());
	//		mEntries.push_back({ MakeOwned<Asset>(), 1 });
	//	}

	//	return AssetRefHandle{index, mEntries[index].generation};
	//}

	AssetRefHandle Allocate(T* Resource)
	{
		uint32 index = 0;
		if (!mFreeIndices.empty())
		{
			index = mFreeIndices.front();
			mFreeIndices.pop();

			mEntries[index].asset.reset(Resource);
			mEntries[index].generation++;
		}
		else
		{
			index = static_cast<uint32>(mEntries.size());
			mEntries.push_back({ OwnedPtr<T>(Resource), 1 });
		}

		return AssetRefHandle{index, mEntries[index].generation};
	}

	template<typename Asset>
	Asset* Get(const AssetRefHandle& Handle)
	{
		static_assert(std::is_base_of<AssetBase, Asset>::value, "Asset must be derived from AssetBase");

		if (!Handle)
		{
			LOG_ERROR("Tried to get asset with invalid handle");
			return nullptr;
		}

		if (Handle.index >= mEntries.size())
		{
			LOG_ERROR("Tried to get asset with index larger then the count of all entries");
			return nullptr;
		}

		const Entry& entry = mEntries[Handle.index];
		if (!entry.asset || entry.generation != Handle.generation)
		{
			LOG_WARNING("Tried to get asset that was either not alive or is of wrong generation (Handle: {}, entry: {})", Handle.generation, entry.generation);
			return nullptr;
		}

		return static_cast<Asset*>(mEntries[Handle.index].asset.get());
	}

	template<typename Asset, typename Fn, typename... Args>
	void Remove(AssetRefHandle& Handle, Fn OnRemove = nullptr, Args&&... args)
	{
		if (Handle.index >= mEntries.size())
		{
			LOG_ERROR("Tried to remove asset with index larger then the count of all entries");
			return;
		}

		Entry& entry = mEntries[Handle.index];
		if (entry.asset && entry.generation == Handle.generation)
		{
			if (OnRemove)
				OnRemove(static_cast<Asset*>(entry.asset.get()), std::forward<Args>(args)...);

			entry.generation++;
			entry.asset = nullptr;
			mFreeIndices.push(Handle.index);

			AssetRefHandle::Invalidate(Handle);
		}
	}

	void Remove(AssetRefHandle& Handle)
	{
		if (Handle.index >= mEntries.size())
		{
			LOG_ERROR("Tried to remove asset with index larger then the count of all entries");
			return;
		}

		Entry& entry = mEntries[Handle.index];
		if (entry.asset && entry.generation == Handle.generation)
		{
			entry.generation++;
			entry.asset = nullptr;
			mFreeIndices.push(Handle.index);

			AssetRefHandle::Invalidate(Handle);
		}
	}

	/**
	* Deletes all assets in this pool.
	*
	* @tparam OnRemove Remove function for specialized deletion of asset.
	* @tparam Args If the OnRemove function needs to take in arguments.
	*/
	template<typename Asset, typename Fn, typename... Args>
	void Clear(Fn OnRemove, Args&&... args)
	{
		for (auto& entry : mEntries)
		{
			if (entry.alive)
			{
				if (OnRemove)
					OnRemove(static_cast<Asset*>(entry.asset.get()), std::forward<Args>(args)...);
			}
		}

		mEntries.clear();
	}

	const Vector<AssetResourcePool<T>::Entry>& GetEntries() { return mEntries; };

private:

	Vector<Entry> mEntries;
	Queue<uint32> mFreeIndices;
};