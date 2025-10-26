#pragma once
#include <Core.h>

/**
* Type safe handle to resources.
*/
template<typename T>
struct ResourceHandle
{
	uint32 index = UINT32_MAX;
	uint32 generation = 0;

	static void Invalidate(ResourceHandle<T>& Handle)
	{ 
		Handle.index = UINT32_MAX;
		Handle.generation = 0; 
	};

	inline static ResourceHandle<T> Invalid() { return { UINT32_MAX, 0 }; };

	operator bool() const
	{
		return index != UINT32_MAX && generation != 0;
	}

	bool operator==(const ResourceHandle<T>& other) const = default;
};


/**
* Non type safe handle used for mostly interfaces where pure virtual functions can't be templated with ResourceHandle.
* 
* @note
* Always prefer ResourceHandle or DirectResourceHandle over GenericHandle as those are type safe and will give compile time errors.
*/
struct GenericHandle
{
	uint32 index = UINT32_MAX;
	uint32 generation = 0;

	static void Invalidate(GenericHandle& Handle)
	{
		Handle.index = UINT32_MAX;
		Handle.generation = 0;
	};

	inline static GenericHandle Invalid() { return { UINT32_MAX, 0 }; };

	operator bool() const
	{
		return index != UINT32_MAX && generation != 0;
	}

	bool operator==(const GenericHandle& other) const = default;
};

/**
* Type safe handle to resources with direct pointer to the resource.
* 
* @note
* When using a DirectResourceHandle the pointer can become invalid. 
* Best practice is to get the resource from the manager or pool directly.
*/
template<typename T>
struct DirectResourceHandle
{
	T* ptr;
	uint32 index = UINT32_MAX;
	uint32 generation = 0;

	static void Invalidate(DirectResourceHandle<T>& Handle)
	{
		Handle.ptr = nullptr;
		Handle.index = UINT32_MAX;
		Handle.generation = 0;
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

template<typename T>
inline ResourceHandle<T> ToResourceHandle(const GenericHandle& Handle)
{
	return ResourceHandle<T>{ Handle.index, Handle.generation };
}

template<typename T>
inline GenericHandle ToGenericHandle(const ResourceHandle<T>& Handle)
{
	return GenericHandle{ Handle.index, Handle.generation };
}