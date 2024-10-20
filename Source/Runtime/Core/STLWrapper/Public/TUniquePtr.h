#pragma once
#include <memory>

template<typename _Val>
class __declspec(dllexport) TUniquePtr
{
public:
    TUniquePtr() : mImpl(nullptr) {};
    explicit TUniquePtr(_Val* aPtr) : mImpl(new Impl(aPtr)) {};

    ~TUniquePtr() { delete mImpl; mImpl = nullptr; };

    TUniquePtr(TUniquePtr&& other) noexcept {
        mImpl = std::move(other.mImpl);
    }

    TUniquePtr(const TUniquePtr&) = delete;
    TUniquePtr& operator=(const TUniquePtr&) = delete;

public:
    _Val* Get()
    {
        return mImpl->ptr.get();
    }

    const _Val* Get() const
    {
        return mImpl->ptr.get();
    }

public:
    _Val* operator->()
    {
        return mImpl->ptr.get();
    }

    const _Val* operator->() const
    {
        return mImpl->ptr.get();
    }

    _Val& operator*() const
    {
        return *(mImpl->ptr);
    }

    TUniquePtr<_Val>& operator=(TUniquePtr<_Val>&& other) noexcept {
        if (this != &other) {
            mImpl = std::move(other.mImpl);
        }
        return *this;
    }

private:
    struct Impl {
        explicit Impl(_Val* aPtr) : ptr(aPtr) {}
        std::unique_ptr<_Val> ptr;
    };
    Impl* mImpl;
};

template<typename _Val, typename... _Args>
inline TUniquePtr<_Val> MakeUnique(_Args&&... args)
{
    return TUniquePtr<_Val>(new _Val(std::forward<_Args>(args)...));
}