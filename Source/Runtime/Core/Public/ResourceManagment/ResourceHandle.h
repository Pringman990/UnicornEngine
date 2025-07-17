#pragma once

struct ScopedHandle
{
	uint32 index = UINT32_MAX;
	uint32 generation = 0;
	std::type_index type;

	bool operator==(const ScopedHandle& other) const = default;
};

template<typename T>
struct ResourceHandle
{
	uint32 index = UINT32_MAX;
	uint32 generation = 0;

	void Invalidate() 
	{ 
		index = UINT32_MAX;
		generation = 0; 
	};

	inline static ResourceHandle<T> Invalid() { return { UINT32_MAX, 0 }; };

	operator bool() const
	{
		return index != UINT32_MAX && generation != 0;
	}

	bool operator==(const ResourceHandle<T>& other) const = default;
};

template<typename T>
using AssetHandle = ResourceHandle<T>;

template<typename T>
using GPUResourceHandle = ResourceHandle<T>;