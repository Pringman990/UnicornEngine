#pragma once
#include <functional>
#include "TSharedPtr.h"

template<typename _ReturnType, typename... _Args>
class __declspec(dllexport) TFunction
{
public:
    // Default constructor
    TFunction() : mImpl(MakeShared<Impl>()) {}
    TFunction(TFunction&& other) noexcept : mImpl(std::move(other.mImpl)) {}

    template<typename Callable>
    TFunction(Callable&& func) : mImpl(MakeShared<Impl>()) {
        // Check if the callable can be assigned
        static_assert(std::is_invocable_r_v<_ReturnType, Callable, _Args...>,
            "Callable must be invocable with the specified arguments.");
        mImpl->function = std::forward<Callable>(func);
    }

    TFunction(const TFunction& other) : mImpl(MakeShared<Impl>()) {
        if (other.mImpl->function) {
            mImpl->function = other.mImpl->function;
        }
    }


public:
    TFunction& operator=(const TFunction& other) {
        if (this != &other) {
            mImpl = MakeShared<Impl>();
            if (other.mImpl->function) {
                mImpl->function = other.mImpl->function;
            }
        }
        return *this;
    }

    TFunction& operator=(TFunction&& other) noexcept {
        if (this != &other) {
            mImpl = std::move(other.mImpl);
        }
        return *this;
    }

    _ReturnType operator()(_Args... args) const {
        //If your getting C2091 function returns function,
        //make sure its not TFunction<RETURNTYPE()>. But Instead TFunction<RETURNTYPE>
        
        if (mImpl->function) {
            if (std::is_void_v<_ReturnType>)
            {
                mImpl->function(std::forward<_Args>(args)...);
                return _ReturnType();
            }
            else
            {
                return mImpl->function(std::forward<_Args>(args)...);
            }
        }
        if constexpr (!std::is_void_v<_ReturnType>) {
            return _ReturnType();
        }
    }

private:
    struct Impl {
        std::function<_ReturnType(_Args...)> function = nullptr;
    };
    TSharedPtr<Impl> mImpl;
};