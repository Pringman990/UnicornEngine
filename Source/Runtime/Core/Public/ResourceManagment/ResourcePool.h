#pragma once
#include "Internal/IResourcePool.h"
#include "ResourceHandle.h"

#include <utility> //std::move

template<typename T>
class ResourcePool final : public IResourcePool
{
private:
	struct Entry
	{
		T resource;
		uint32 generation;
		bool alive = false;
	};

public:
	ResourcePool() = default;
	~ResourcePool() override 
	{
		for (auto& entry : mEntries)
		{
			T::Free(entry.resource);
		}
	};

	const ResourceHandle<T> Allocate()
	{
		uint32 index = 0;
		if (!mFreeIndices.empty())
		{
			index = mFreeIndices.front();
			mFreeIndices.pop();

			mEntries[index].resource = {};
			mEntries[index].generation++;
			mEntries[index].alive = true;
		}
		else
		{
			index = static_cast<uint32>(mEntries.size());
			mEntries.push_back({ {}, 1, true });
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

	T* Get(const ScopedHandle& Handle)
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

	void Remove(ResourceHandle<T>& Handle, Func<void(T*)> OnRemove = nullptr)
	{
		if (Handle.index >= mEntries.size())
		{
			_LOG_CORE_ERROR("Tried to remove resource with index larger then the count of all entries");
			return;
		}

		Entry& entry = mEntries[Handle.index];
		if (entry.alive && entry.generation == Handle.generation)
		{
			if(OnRemove)
				OnRemove(&entry.resource);
			
			entry.alive = false;
			entry.generation++;
			entry.resource = {};
			mFreeIndices.push(Handle.index);
			
			Handle.Invalidate();
		}
	}

private:
	Vector<Entry> mEntries;
	Queue<uint32> mFreeIndices;
};

