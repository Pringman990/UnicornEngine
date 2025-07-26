#pragma once
#include <Core.h>

#include <ResourceManagment/ResourcePool.h>


class IGPUResourceFactory;

class GPUResourceManager
{
public:
	using ResourceUploadTask = Func<void(CommandBuffer*)>;
public:
	GPUResourceManager();
	~GPUResourceManager();

	template<typename _Resource>
	void RegisterPool()
	{
		auto it = mPools.find(typeid(_Resource));
		if (it != mPools.end())
			_ASSERT_RENDERER(false, "Pool for resource type: {}, already exists!", typeid(_Resource).name());

		mPools[typeid(_Resource)] = OwnedPtr<IResourcePool>(new ResourcePool<_Resource>());
	}

	template<typename T>
	void UnRegisterPool()
	{
		auto it = mPools.find(typeid(T));
		if (it == mPools.end())
			return;

		mPools.erase(it);
	}

	template<typename T>
	const GPUResourceHandle<T> AllocateResource()
	{
		ResourcePool<T>* pool = GetPool<T>();
		if (!pool)
		{
			_THROW_CORE("No gpu resource pool for type");
			return GPUResourceHandle<T>::Invalid();
		}

		return pool->Allocate();
	};

	template<typename T>
	T* GetResource(const GPUResourceHandle<T>& Handle)
	{
		ResourcePool<T>* pool = GetPool<T>();
		if (!pool)
		{
			_ENSURE_CORE(pool, "No resource pool for type");
			return nullptr;
		}

		return pool->Get(Handle);
	}

	template<typename T>
	T* GetResource(const ScopedHandle& Handle)
	{
		ResourcePool<T>* pool = GetPool<T>();
		if (!pool)
		{
			_ENSURE_CORE(pool, "No resource pool for type");
			return nullptr;
		}

		return pool->Get(Handle);
	}

	template<typename T>
	void FreeResource(GPUResourceHandle<T>& Handle, LogicalDevice* Device)
	{
		ResourcePool<T>* pool = GetPool<T>();
		if (!pool)
		{
			_ENSURE_CORE(pool, "No resource pool for type");
			return;
		}

		pool->Remove(Handle, [Device](T* resource) { T::Free(resource, Device->GetRaw());});
	}

	template<typename T>
	void FreeResource(GPUResourceHandle<T>& Handle, LogicalDevice* Device, Func<void(T*, VkDevice)> RemoveFunc)
	{
		ResourcePool<T>* pool = GetPool<T>();
		if (!pool)
		{
			_ENSURE_CORE(pool, "No resource pool for type");
			return;
		}

		pool->Remove(Handle, [Device, RemoveFunc](T* resource) { RemoveFunc(resource, Device->GetRaw());});
	}

	void UploadTask(ResourceUploadTask&& Task)
	{
		std::lock_guard lock(mMutex);
		mCreationQueue[mCurrentQueueIndex].push(Task);
	}

	void SwapQueues()
	{
		mCurrentQueueIndex = (mCurrentQueueIndex + 1) % 2;
	}

	void ExecuteUploads(CommandBuffer* Cmd)
	{
		while (!mCreationQueue[mCurrentQueueIndex].empty())
		{
			auto& func = mCreationQueue[mCurrentQueueIndex].front();
			func(Cmd);
			mCreationQueue[mCurrentQueueIndex].pop();
		}
	}

private:
	template<typename T>
	ResourcePool<T>* GetPool()
	{
		auto it = mPools.find(typeid(T));
		if (it == mPools.end())
			return nullptr;

		return static_cast<ResourcePool<T>*>(mPools[typeid(T)].get());
	}

private:
	UnorderedMap<std::type_index, OwnedPtr<IResourcePool>> mPools;

	std::mutex mMutex;
	uint32 mCurrentQueueIndex = 0;
	Queue<ResourceUploadTask> mCreationQueue[2];
};