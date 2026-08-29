#pragma once
#include "Types.h"

template<typename T, typename M>
constexpr std::ptrdiff_t OffsetOf(M T::* member)
{
    return reinterpret_cast<std::ptrdiff_t>(
        &(reinterpret_cast<T*>(0)->*member)
    );
}

constexpr void FromUint64(const uint64 v, uint32& lower, uint32& upper)
{
    lower = static_cast<uint32>((v & 0xFFFFFFFF00000000LL) >> 32);
    upper = static_cast<uint32>(v & 0xFFFFFFFFLL);
}

constexpr uint64 ToUint64(const uint32 lower, const uint32 upper)
{
    const uint64 result = static_cast<uint64>(lower) << 32 | upper;
    return result;
}