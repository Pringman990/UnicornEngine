#pragma once
#include <string>
#include "TUniquePtr.h"

class __declspec(dllexport) WideString {
public:
	WideString() : mImpl(MakeUnique<Impl>()) {}
	WideString(const WideString& aString) : mImpl(MakeUnique<Impl>()) { mImpl->str = aString.Get(); }
	WideString(const wchar_t* aCharPtr) : mImpl(MakeUnique<Impl>()) { mImpl->str = aCharPtr; }
	~WideString() = default;

	size_t Size() const {
		return mImpl->str.size();
	}

	void Clear() {
		mImpl->str.clear();
	}

public:
	void operator=(const WideString& aOther)
	{
		this->mImpl->str = aOther.mImpl->str;
	}
	void operator=(const wchar_t* aCharPtr)
	{
		this->mImpl->str = aCharPtr;
	}

	bool operator==(const WideString& aOther) const
	{
		return this->mImpl->str == aOther.mImpl->str;
	}
	bool operator==(const wchar_t* aCharPtr) const
	{
		return this->mImpl->str == aCharPtr;
	}

	void operator+=(const WideString& aOther)
	{
		this->mImpl->str += aOther.mImpl->str;
	}
	void operator+=(const wchar_t* aCharPtr)
	{
		this->mImpl->str += aCharPtr;
	}

	WideString& operator+(const WideString& aOther)
	{
		this->mImpl->str += aOther.mImpl->str;
		return *this;
	}
	WideString& operator+(const wchar_t* aCharPtr)
	{
		this->mImpl->str += aCharPtr;
		return *this;
	}

	const std::wstring& Get() const
	{
		return mImpl->str;
	}

	std::wstring& Get()
	{
		return mImpl->str;
	}
private:
	struct Impl {
		explicit Impl(const std::wstring& s = L"") : str(s) {}
		explicit Impl(const wchar_t* s) : str(s) {}
		std::wstring str;
	};

	TUniquePtr<Impl> mImpl;
};

namespace std {
	template <>
	struct hash<WideString> {
		std::size_t operator()(const WideString& s) const noexcept {
			// Use std::hash<std::string> to hash the internal string
			return std::hash<std::wstring>()(s.Get());
		}
	};
}