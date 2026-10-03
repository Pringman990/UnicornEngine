#pragma once

#include <climits>
#include <cstring>

#include "Core/Types.h"
#include "Core/UniqueID128.h"
#include "Core/GenerationHandle.h"

using AssetID = UniqueID128;
using AssetTypeID = String;

class Asset
{
public:
    Asset() = default;
    virtual ~Asset() = default;
    Asset(const Asset&) = delete;
    Asset& operator=(const Asset&) = delete;

    virtual StringView GetType() const = 0;
};

// Persistent identity, serialized by the ECS as a UUID.
template<typename T>
struct AssetRef
{
    AssetID id{};
};

// Session-only identity for direct slot lookup.
template<typename T = Asset>
using AssetHandle = GenerationHandle<T>;

struct AssetInfo
{
    AssetID id;
    AssetTypeID type;
    String path;
};

#define INIT_ASSET(typeName) \
    public: \
        static constexpr StringView TYPE = typeName; \
        StringView GetType() const override { return TYPE; } \
    private:
