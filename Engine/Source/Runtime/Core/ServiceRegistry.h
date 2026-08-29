#pragma once
#include <assert.h>

using ServiceID = uint64;

constexpr uint64 FNV_OFFSET = 14695981039346656037ull;
constexpr uint64 FNV_PRIME  = 1099511628211ull;

constexpr ServiceID Hash(const char* str)
{
    uint64 hash = FNV_OFFSET;

    while (*str)
    {
        hash ^= static_cast<byte>(*str++);
        hash *= FNV_PRIME;
    }

    return hash;
}

constexpr ServiceID ToServiceID(const StringView str)
{
    return Hash(str.data());
}

template<typename T>
concept ValidService =
requires
{
    { T::ID } -> std::same_as<const ServiceID&>;
};

class ServiceRegistry
{
    struct ServiceEntry
    {
        void* ptr = nullptr;
    };

public:
    ServiceRegistry() = default;
    ServiceRegistry(const ServiceRegistry&) = delete;
    ServiceRegistry& operator=(const ServiceRegistry&) = delete;

    template<ValidService T>
    void RegisterService(T* service)
    {
        auto it = mLookup.find(T::ID);

        if (it != mLookup.end())
        {
            // Hot reload
            mServices[it->second].ptr = service;
            return;
        }

        auto index = static_cast<uint32>(mServices.size());
        mLookup.emplace(T::ID, index);
        mServices.push_back({ service,  });
    }

    template<ValidService T>
    void InvalidateService()
    {
        auto index = FindIndex<T>();

        mServices[index].ptr = nullptr;
    }

    template<ValidService T>
    T& GetService()
    {
        static const auto index = FindIndex<T>();
        assert(mServices[index].ptr);
        return *static_cast<T*>(mServices[index].ptr);
    }

    template<ValidService T>
    T* TryGetService()
    {
        static const auto index = FindIndex<T>();
        auto& [ptr] = mServices[index];
        return static_cast<T*>(ptr);
    }

private:
    template<typename T>
    NODISC uint32_t FindIndex()
    {
        auto it = mLookup.find(T::ID);

        assert(it != mLookup.end());

        return it->second;
    }

private:
    List<ServiceEntry> mServices;
    UnorderedMap<ServiceID, uint32> mLookup;
};

extern ServiceRegistry* GServiceRegistry;

namespace Services
{
    template<ValidService T>
    T& Get()
    {
        return GServiceRegistry->GetService<T>();
    }

    template<ValidService T>
    T* TryGet()
    {
        return GServiceRegistry->TryGetService<T>();
    }
}

#define INIT_SERVICE(service) \
    public: \
        static constexpr ServiceID ID = ToServiceID(#service); \
    private: