#pragma once
#include "Internal/IAssetPool.h"
#include "AssetHandle.h"

#include <utility> //std::move

template<typename T>
class AssetPool : public IAssetPool
{
public:
	AssetPool() = default;
	~AssetPool() = default;

	AssetHandle<T> Add(T&& Asset)
	{
		uint32 index = 0;
		if (!mFreeIndices.empty())
		{
			index = mFreeIndices.front();
			mFreeIndices.pop();

			mEntries[index].asset = std::move(Asset);
			mEntries[index].generation++;
			mEntries[index].alive = true;
		}
		else
		{
			index = static_cast<uint32>(mEntries.size());
			mEntries.push_back({std::move(Asset), 1, true});
		}

		return AssetHandle<T>{index, mEntries[index].generation};
	}

	T* Get(const AssetHandle<T>& Handle)
	{
		if (Handle.index >= mEntries.size())
		{
			_LOG_CORE_ERROR("Tried to get asset with index larger then the count of all entries");
			return nullptr;
		}

		const Entry& entry = mEntries[Handle.index];
		if (!entry.alive || entry.generation != Handle.generation)
		{
			_LOG_CORE_WARNING("Tried to get asset that was either not alive ({}) or is of wrong generation (Handle: {}, entry: {})", entry.alive, Handle.generation, entry.generation);
			return nullptr;
		}

		return &mEntries[Handle.index].asset;
	}

	void Remove(const AssetHandle<T>& Handle)
	{
		if (Handle.index >= mEntries.size())
		{
			_LOG_CORE_ERROR("Tried to remove asset with index larger then the count of all entries");
			return;
		}

		const Entry& entry = mEntries[Handle.index];
		if (entry.alive && entry.generation == Handle.generation)
		{
			entry.alive = false;
			entry.generation++;
			mFreeIndices.push(Handle.index);
		}
	}

private:
	struct Entry
	{
		T asset;
		uint32 generation;
		bool alive = false;
	};

	Vector<Entry> mEntries;
	Queue<uint32> mFreeIndices;
};

