#pragma once
#include <Core.h>

#include "Internal/IResourcePool.h"
#include "Logger/Logger.h"
#include "ResourceHandle.h"

#include <utility> //std::move

/**
* Resource pool where all resources are on the heap.
* Used for safe access with WeakPtr.
*/
template<typename T>
class HeapResourcePool : public IResourcePool
{
public:
	struct Entry
	{
		SharedPtr<T> resource;
		uint32 generation;
	};

	HeapResourcePool() = default;
	~HeapResourcePool() override = default;

	const ResourceHandle<T> Allocate()
	{
		uint32 index = 0;
		if (!mFreeIndices.empty())
		{
			index = mFreeIndices.front();
			mFreeIndices.pop();

			mEntries[index].resource = MakeShared<T>();
			mEntries[index].generation++;
		}
		else
		{
			index = static_cast<uint32>(mEntries.size());
			mEntries.push_back({ MakeShared<T>(), 1 });
		}

		return ResourceHandle<T>{index, mEntries[index].generation};
	}

	template<typename args>
	const ResourceHandle<T> Allocate(args Args...)
	{
		uint32 index = 0;
		if (!mFreeIndices.empty())
		{
			index = mFreeIndices.front();
			mFreeIndices.pop();

			mEntries[index].resource = MakeShared<T>(Args);
			mEntries[index].generation++;
		}
		else
		{
			index = static_cast<uint32>(mEntries.size());
			mEntries.push_back({ MakeShared<T>(), 1 });
		}

		return ResourceHandle<T>{index, mEntries[index].generation};
	}

	WeakPtr<T> Get(const ResourceHandle<T>& Handle)
	{
		if (!Handle)
		{
			LOG_ERROR("Tried to get resource with invalid handle");
			return WeakPtr<T>();
		}

		if (Handle.index >= mEntries.size())
		{
			LOG_ERROR("Tried to get resource with index larger then the count of all entries");
			return WeakPtr<T>();
		}

		const Entry& entry = mEntries[Handle.index];
		if (!entry.resource || entry.generation != Handle.generation)
		{
			LOG_WARNING("Tried to get resource that was either not alive or is of wrong generation (Handle: {}, entry: {})", Handle.generation, entry.generation);
			return WeakPtr<T>();
		}

		return mEntries[Handle.index].resource;
	}

	template<typename Fn, typename... Args>
	void Remove(ResourceHandle<T>& Handle, Fn OnRemove = nullptr, Args&&... args)
	{
		if (Handle.index >= mEntries.size())
		{
			LOG_ERROR("Tried to remove resource with index larger then the count of all entries");
			return;
		}

		Entry& entry = mEntries[Handle.index];
		if (entry.resource && entry.generation == Handle.generation)
		{
			if (OnRemove)
				OnRemove(entry.resource.get(), std::forward<Args>(args)...);

			entry.generation++;
			entry.resource = nullptr;
			mFreeIndices.push(Handle.index);

			ResourceHandle<T>::Invalidate(Handle);
		}
	}

	void Remove(ResourceHandle<T>& Handle)
	{
		if (Handle.index >= mEntries.size())
		{
			LOG_ERROR("Tried to remove resource with index larger then the count of all entries");
			return;
		}

		Entry& entry = mEntries[Handle.index];
		if (entry.resource && entry.generation == Handle.generation)
		{
			entry.generation++;
			entry.resource = nullptr;
			mFreeIndices.push(Handle.index);

			ResourceHandle<T>::Invalidate(Handle);
		}
	}

	/**
	* Deletes all resources in this pool.
	*
	* @tparam OnRemove Remove function for specialized deletion of resource.
	* @tparam Args If the OnRemove function needs to take in arguments.
	*/
	template<typename Fn, typename... Args>
	void Clear(Fn OnRemove, Args&&... args)
	{
		for (auto& entry : mEntries)
		{
			if (entry.alive)
			{
				if (OnRemove)
					OnRemove(entry.resource.get(), std::forward<Args>(args)...);
			}
		}

		mEntries.clear();
	}

	const Vector<HeapResourcePool<T>::Entry>& GetEntries() { return mEntries; };

private:

	Vector<Entry> mEntries;
	Queue<uint32> mFreeIndices;
};