#pragma once
#include <Core.h>

#include <ResourceManagment/ResourcePool.h>

template<typename T>
struct function_traits;

// Function pointer
template<typename Ret, typename First, typename... Rest>
struct function_traits<Ret(*)(First, Rest...)> 
{
	using first_argument = First;
};

class IGPUResourceFactory;

class GPUResourceManager
{
public:
	using ResourceUploadTask = Func<void(CommandBuffer*)>;
public:
	GPUResourceManager() = default;
	~GPUResourceManager()
	{
		_ASSERT_RENDERER(mPools.size() <= 0, "There are still pools in the Manager, Please call UnRegisterPool");
	};

	template<typename _Resource>
	void RegisterPool()
	{
		auto it = mPools.find(typeid(_Resource));
		if (it != mPools.end())
			_ASSERT_RENDERER(false, "Pool for resource type: {}, already exists!", typeid(_Resource).name());

		mPools[typeid(_Resource)] = OwnedPtr<IResourcePool>(new ResourcePool<_Resource>());
	}

	template<typename T, typename Fn, typename... Args>
	void UnRegisterPool(Fn OnRemove, Args&&... args)
	{
		if constexpr (std::is_pointer_v<Fn>)
		{
			static_assert(std::is_same_v<
				typename function_traits<Fn>::first_argument,
				T*>, "OnRemove must take T* as the first parameter.");
		}

		auto it = mPools.find(typeid(T));
		if (it == mPools.end())
			return;

		ResourcePool<T>* pool = GetPool<T>();
		pool->Clear(OnRemove, std::forward<Args>(args)...);

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

	template<typename T, typename Fn, typename... Args>
	void FreeResource(const GPUResourceHandle<T>& Handle, Fn OnRemove, Args&&... args)
	{
		if constexpr (std::is_pointer_v<Fn>)
		{
			static_assert(std::is_same_v<
				typename function_traits<Fn>::first_argument,
				T*>, "OnRemove must take T* as the first parameter.");
		}

		auto it = mPools.find(typeid(T));
		if (it == mPools.end())
			return;

		ResourcePool<T>* pool = GetPool<T>();
		pool->Clear(OnRemove, std::forward<Args>(args)...);

		mPools.erase(it);
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