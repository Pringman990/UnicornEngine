#pragma once
#include <vector>
#include "TSharedPtr.h"

template<typename _Val>
class __declspec(dllexport) TVector
{
public:
    TVector() : mImpl(MakeShared<Impl>()) {};
    TVector(TVector&& other) noexcept : mImpl(std::move(other.mImpl)) {}

    TVector(const TVector& other) : mImpl(MakeShared<Impl>()) {
        mImpl->vector = other.mImpl->vector;
    }

    ~TVector() = default;

public:
    //Adds element to the back of the vector
    void Add(const _Val& aValue)
    {
        mImpl->vector.push_back(aValue);
    }

    //Adds element to the back of the vector
    void Add(_Val&& aValue)
    {
        mImpl->vector.push_back(std::move(aValue));
    }

    /// <summary>
    /// Adds element to the back of the vector
    /// </summary>
    /// <returns>
    /// TRUE if value was not already in the vector.
    /// FALSE if value already exists
    /// </returns>
    bool AddUnique(const _Val& aValue)
    {
        auto it = std::find_if(mImpl->vector.begin(), mImpl->vector.end(),
            [this, &aValue](const auto& elem) { // Explicitly capture 'this'
                return IsEqual(elem, aValue);
            });

        if (it == mImpl->vector.end()) {
            mImpl->vector.push_back(std::move(aValue));
            return true;
        }
        return false;
    }
 
    /// <summary>
    /// Adds element to the back of the vector
    /// </summary>
    /// <returns>
    /// TRUE if value was not already in the vector.
    /// FALSE if value already exists
    /// </returns>
    bool AddUnique(_Val&& aValue)
    {
        auto it = std::find(mImpl->vector.begin(), mImpl->vector.end(), aValue);
        if (it == mImpl->vector.end())
        {
            mImpl->vector.push_back(std::move(aValue));
            return true;
        }
        return false;
    }

    size_t Size() const
    {
        return mImpl->vector.size();
    }

    void Erase(const _Val& aValue)
    {
        auto it = std::find(mImpl->vector.begin(), mImpl->vector.end(), aValue);
        if (it != mImpl->vector.end())
        {
            mImpl->vector.erase(it);
        }
    }

    void Clear()
    {
        mImpl->vector.clear();
    }

public:
    typename std::vector<_Val>::iterator begin() {
        return mImpl->vector.begin();
    }

    typename std::vector<_Val>::iterator end() {
        return mImpl->vector.end();
    }

    // Const versions for use with const TVector objects
    typename std::vector<_Val>::const_iterator begin() const {
        return mImpl->vector.begin();
    }

    typename std::vector<_Val>::const_iterator end() const {
        return mImpl->vector.end();
    }

public:
    _Val& operator[](const int32_t& aIndex)
    {
        return mImpl->vector[aIndex];
    }

    TVector& operator=(TVector&& other) noexcept {
        if (this != &other) {
            mImpl = std::move(other.mImpl);
        }
        return *this;
    }

    TVector& operator=(const TVector& other) {
        if (this != &other) {
            mImpl->vector = other.mImpl->vector;
        }
        return *this;
    }

private:
    template <typename T, typename U>
    bool IsEqual(const T& lhs, const U& rhs) const {
        // Compare based on type and pointer if they are both callable
        return typeid(lhs) == typeid(rhs) && std::addressof(lhs) == std::addressof(rhs);
    }
private:
    struct Impl {
        std::vector<_Val> vector;
    };
    TSharedPtr<Impl> mImpl;
};