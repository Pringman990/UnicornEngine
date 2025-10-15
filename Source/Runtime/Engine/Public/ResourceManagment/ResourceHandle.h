#pragma once

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
struct DirectResourceHandle
{
	T* ptr;
	uint32 index = UINT32_MAX;
	uint32 generation = 0;

	void Invalidate()
	{
		ptr = nullptr;
		index = UINT32_MAX;
		generation = 0;
	};

	inline static DirectResourceHandle<T> Invalid() { return { nullptr, UINT32_MAX, 0 }; };

	operator bool() const
	{
		return index != UINT32_MAX && ptr != nullptr && generation != 0;

	}

	bool operator==(const DirectResourceHandle<T>& other) const = default;
};

template<typename T>
using AssetHandle = ResourceHandle<T>;

template<typename T>
using GPUResourceHandle = ResourceHandle<T>;