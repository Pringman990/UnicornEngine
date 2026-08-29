#pragma once

template <typename Tag>
struct GenerationHandle
{
    uint32 index = UINT_MAX;
    uint32 generation = 0;

    operator bool() const
    {
        return index != UINT_MAX && generation != 0;
    }
};
