#pragma once
#include <memory>

template<typename _Val>
class __declspec(dllexport) TSharedPtr
{
public:
    TSharedPtr() : mImpl(nullptr) {};
    explicit TSharedPtr(_Val* aPtr) : mImpl(new Impl(aPtr)) {};

    ~TSharedPtr() { delete mImpl; mImpl = nullptr; };

    TSharedPtr(TSharedPtr&& other) noexcept : mImpl(other.mImpl) {
        other.mImpl = nullptr;
    }

    TSharedPtr(const TSharedPtr& other) {
        if (other.mImpl) {
            mImpl = new Impl(*other.mImpl);
        }
        else {
            mImpl = nullptr;
        }
    }

public:
    _Val* Get()
    {
        return mImpl ? mImpl->ptr.get() : nullptr;
    }

public:
    _Val* operator->()
    {
        return mImpl ? mImpl->ptr.get() : nullptr;
    }

    const _Val* operator->() const
    {
        return mImpl ? mImpl->ptr.get() : nullptr;
    }

    _Val& operator*() const
    {
        return *(mImpl->ptr);
    }

    TSharedPtr& operator=(TSharedPtr&& other) noexcept {
        if (this != &other) {
            delete mImpl;
            mImpl = other.mImpl;
            other.mImpl = nullptr;
        }
        return *this;
    }

    TSharedPtr<_Val>& operator=(std::shared_ptr<_Val>&& other) noexcept {
        if (mImpl == nullptr)
        {
            mImpl = new Impl();
        }
        mImpl->ptr = other;
        return *this;
    }

private:
    struct Impl {
        Impl() : ptr(nullptr) {};
        explicit Impl(_Val* aPtr) : ptr(aPtr) {}
        std::shared_ptr<_Val> ptr;
    };
    Impl* mImpl;
};

template<typename _Val, typename... _Args>
inline TSharedPtr<_Val> MakeShared(_Args&&... args)
{
    return TSharedPtr<_Val>(new _Val(std::forward<_Args>(args)...));
}

template<typename _Val, typename... _Args>
inline TSharedPtr<_Val> MakeShared(_Val* ptr, _Args&&... args)
{
    return TSharedPtr<_Val>(new (ptr) _Val(std::forward<_Args>(args)...));
}