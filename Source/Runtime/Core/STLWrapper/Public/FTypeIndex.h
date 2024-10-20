#pragma once
#include <typeindex>
#include "TUniquePtr.h"

class __declspec(dllexport) FTypeIndex
{
public:

    ~FTypeIndex() = default;

    template<class _Class>
    static FTypeIndex Create()
    {
        return FTypeIndex(typeid(_Class));
    }

    const std::type_index& Get() const
    {
        return mImpl->typeIndex;
    }

public:
    // Compare type indexes
    bool operator==(const FTypeIndex& other) const {
        return mImpl->typeIndex == other.mImpl->typeIndex;
    }

    bool operator!=(const FTypeIndex& other) const {
        return mImpl->typeIndex != other.mImpl->typeIndex;
    }

private:
    FTypeIndex(const std::type_index& aType) : mImpl(MakeUnique<Impl>(aType)) { }

private:
    struct Impl {
        explicit Impl(const std::type_index& typeIdx) : typeIndex(typeIdx) {}
        std::type_index typeIndex;
    };

    TUniquePtr<Impl> mImpl;
};

namespace std {
    template <>
    struct hash<FTypeIndex> {
        std::size_t operator()(const FTypeIndex& s) const noexcept {
            return s.Get().hash_code();
        }
    };
}