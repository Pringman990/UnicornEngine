#pragma once
#include <string>
#include "TUniquePtr.h"

class __declspec(dllexport) FString {
public:
	FString() : mImpl(MakeUnique<Impl>()) {}
	FString(const FString& aString) : mImpl(MakeUnique<Impl>()) { mImpl->str = aString.Get(); }
	FString(const char* aCharPtr) : mImpl(MakeUnique<Impl>()) { mImpl->str = aCharPtr; }
	~FString() = default;

	size_t Size() const {
		return mImpl->str.size();
	}

	void Clear() {
		mImpl->str.clear();
	}
public:
	void operator=(const FString& aOther)
	{
		this->mImpl->str = aOther.mImpl->str;
	}
	void operator=(const char* aCharPtr)
	{
		this->mImpl->str = aCharPtr;
	}

	bool operator==(const FString& aOther) const
	{
		return this->mImpl->str == aOther.mImpl->str;
	}
	bool operator==(const char* aCharPtr) const
	{
		return this->mImpl->str == aCharPtr;
	}

	void operator+=(const FString& aOther)
	{
		this->mImpl->str += aOther.mImpl->str;
	}
	void operator+=(const char* aCharPtr)
	{
		this->mImpl->str += aCharPtr;
	}

	FString& operator+(const FString& aOther)
	{
		this->mImpl->str += aOther.mImpl->str;
		return *this;
	}
	FString& operator+(const char* aCharPtr)
	{
		this->mImpl->str += aCharPtr;
		return *this;
	}

	const std::string& Get() const
	{
		return mImpl->str;
	}
private:
	struct Impl {
		explicit Impl(const std::string& s = "") : str(s) {}
		explicit Impl(const char* s) : str(s) {}
		std::string str;
	};

	TUniquePtr<Impl> mImpl;
};

namespace std {
	template <>
	struct hash<FString> {
		std::size_t operator()(const FString& s) const noexcept {
			// Use std::hash<std::string> to hash the internal string
			return std::hash<std::string>()(s.Get());
		}
	};
}