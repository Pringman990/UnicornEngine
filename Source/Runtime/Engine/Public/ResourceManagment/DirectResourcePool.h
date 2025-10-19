#pragma once
#include "Logger/Logger.h"
#include "ResourceHandle.h"

#include <utility> //std::move

/**
* Resource pool with direct pointer access to the resource.
*/
template<typename T>
class DirectResourcePool
{
public:
	struct Entry
	{
		T resource;
		uint32 generation;
		bool alive = false;
	};

	DirectResourcePool() = default;
	~DirectResourcePool() {};
	
	DirectResourceHandle<T> Allocate()
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

		return DirectResourceHandle<T>{&mEntries[index].resource, index, mEntries[index].generation};
	}

	DirectResourceHandle<T> Allocate(T& Resource)
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
			mEntries.push_back({ std::move(Resource), 1, true });
		}

		return DirectResourceHandle<T>{&mEntries[index].resource, index, mEntries[index].generation};
	}

	DirectResourceHandle<T> Get(const DirectResourceHandle<T>& Handle)
	{
		if (Handle.index >= mEntries.size())
		{
			LOG_ERROR("Tried to get resource with index larger then the count of all entries");
			return nullptr;
		}

		const Entry& entry = mEntries[Handle.index];
		if (!entry.alive || entry.generation != Handle.generation)
		{
			LOG_WARNING("Tried to get resource that was either not alive ({}) or is of wrong generation (Handle: {}, entry: {})", entry.alive, Handle.generation, entry.generation);
			return nullptr;
		}

		return DirectResourceHandle<T>{&mEntries[Handle.index].resource, Handle.index, mEntries[Handle.index].generation};
	}

	template<typename Fn, typename... Args>
	void Remove(DirectResourceHandle<T>& Handle, Fn OnRemove, Args&&... args)
	{
		if (Handle.index >= mEntries.size())
		{
			LOG_ERROR("Tried to remove resource with index larger then the count of all entries");
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
					OnRemove(&entry.resource, std::forward<Args>(args)...);
			}
		}

		mEntries.clear();
	}

	const Vector<DirectResourcePool<T>::Entry>& GetEntries() { return mEntries; };

private:

	Deque<Entry> mEntries;
	Queue<uint32> mFreeIndices;
};
