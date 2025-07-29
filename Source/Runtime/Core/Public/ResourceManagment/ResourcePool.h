#pragma once
#include "Internal/IResourcePool.h"
#include "ResourceHandle.h"

#include <utility> //std::move

template<typename T>
class ResourcePool : public IResourcePool
{
public:
	struct Entry
	{
		T resource;
		uint32 generation;
		bool alive = false;
	};

	ResourcePool() = default;
	virtual ~ResourcePool() override {};

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

	template<typename Fn, typename... Args>
	void Remove(ResourceHandle<T>& Handle, Fn OnRemove, Args&&... args)
	{
		if (Handle.index >= mEntries.size())
		{
			_LOG_CORE_ERROR("Tried to remove resource with index larger then the count of all entries");
			return;
		}

		const Entry& entry = mEntries[Handle.index];
		if (entry.alive && entry.generation == Handle.generation)
		{
			if (OnRemove)
				OnRemove(&entry.resource, std::forward<Args>(args)...);

			entry.alive = false;
			entry.generation++;
			entry.resource = {};
			mFreeIndices.push(Handle.index);

			Handle.Invalidate();
		}
	}

	/// <summary>
	/// Deletes all resources in this pool
	/// </summary>
	/// <typeparam name="...Args"></typeparam>
	/// <param name="Handle"></param>
	/// <param name="OnRemove"></param>
	/// <param name="...args"></param>
	template<typename Fn, typename... Args>
	void Clear(Fn OnRemove, Args&&... args)
	{
		for (auto& entry : mEntries)
		{
			if (entry.alive)
			{
				if (OnRemove)
					OnRemove(&entry.resource, std::forward<Args>(args)...);
			}
		}

		mEntries.clear();
	}

	const Vector<ResourcePool<T>::Entry>& GetEntries() { return mEntries; };

private:

	Vector<Entry> mEntries;
	Queue<uint32> mFreeIndices;
};

