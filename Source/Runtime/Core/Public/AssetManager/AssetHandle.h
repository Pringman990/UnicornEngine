#pragma once

template<typename T>
struct AssetHandle
{
	uint32 index = UINT32_MAX;
	uint32 generation = 0;

	inline static AssetHandle<T> Invalid() { return { UINT32_MAX, 0 }; };

	operator bool() const
	{
		return index != UINT32_MAX && generation != 0;
	}

	bool operator==(const AssetHandle& other) const = default;
};