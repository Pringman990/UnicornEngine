#pragma once
#include "Internal/IResourcePool.h"
#include "ResourceHandle.h"

#include <utility> //std::move

template<typename T>
class ResourcePool final : public IResourcePool
{
public:
	ResourcePool() = default;
	~ResourcePool() = default;

	ResourceHandle<T> Add(T&& Resource)
	{
		uint32 index = 0;
		if (!mFreeIndices.empty())
		{
			index = mFreeIndices.front();
			mFreeIndices.pop();

			mEntries[index].resource = std::move(Resource);
			mEntries[index].generation++;
			mEntries[index].alive = true;
		}
		else
		{
			index = static_cast<uint32>(mEntries.size());
			mEntries.push_back({std::move(Resource), 1, true});
		}

		return ResourceHandle<T>{index, mEntries[index].generation};
	}

	T* Get(const ResourceHandle<T>& Handle)
	{
		if (Handle.index >= mEntries.size())
		{
			_LOG_CORE_ERROR("Tried to get resource with index larger then the count of all entries");
			return nullptr;
		}

		const Entry& entry = mEntries[Handle.index];
		if (!entry.alive || entry.generation != Handle.generation)
		{
			_LOG_CORE_WARNING("Tried to get resource that was either not alive ({}) or is of wrong generation (Handle: {}, entry: {})", entry.alive, Handle.generation, entry.generation);
			return nullptr;
		}

		return &mEntries[Handle.index].resource;
	}

	void Remove(const ResourceHandle<T>& Handle)
	{
		if (Handle.index >= mEntries.size())
		{
			_LOG_CORE_ERROR("Tried to remove resource with index larger then the count of all entries");
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
		T resource;
		uint32 generation;
		bool alive = false;
	};

	Vector<Entry> mEntries;
	Queue<uint32> mFreeIndices;
};

