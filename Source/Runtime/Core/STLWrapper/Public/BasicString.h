#pragma once
#include <string>
#include "TUniquePtr.h"

class __declspec(dllexport) BasicString {
public:
	BasicString() : mImpl(MakeUnique<Impl>()) {}
	BasicString(const BasicString& aString) : mImpl(MakeUnique<Impl>()) { mImpl->str = aString.Get(); }
	BasicString(const char* aCharPtr) : mImpl(MakeUnique<Impl>()) { mImpl->str = aCharPtr; }
	~BasicString() = default;

	size_t Size() const {
		return mImpl->str.size();
	}

	void Clear() {
		mImpl->str.clear();
	}
public:
	void operator=(const BasicString& aOther)
	{
		this->mImpl->str = aOther.mImpl->str;
	}
	void operator=(const char* aCharPtr)
	{
		this->mImpl->str = aCharPtr;
	}

	bool operator==(const BasicString& aOther) const
	{
		return this->mImpl->str == aOther.mImpl->str;
	}
	bool operator==(const char* aCharPtr) const
	{
		return this->mImpl->str == aCharPtr;
	}

	void operator+=(const BasicString& aOther)
	{
		this->mImpl->str += aOther.mImpl->str;
	}
	void operator+=(const char* aCharPtr)
	{
		this->mImpl->str += aCharPtr;
	}

	BasicString& operator+(const BasicString& aOther)
	{
		this->mImpl->str += aOther.mImpl->str;
		return *this;
	}
	BasicString& operator+(const char* aCharPtr)
	{
		this->mImpl->str += aCharPtr;
		return *this;
	}

	const std::string& Get() const
	{
		return mImpl->str;
	}

	std::string& Get()	{
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
	struct hash<BasicString> {
		std::size_t operator()(const BasicString& s) const noexcept {
			// Use std::hash<std::string> to hash the internal string
			return std::hash<std::string>()(s.Get());
		}
	};
}