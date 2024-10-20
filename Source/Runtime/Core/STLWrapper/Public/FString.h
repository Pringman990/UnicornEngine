#pragma once
#include "BasicString.h"
#include "WideString.h"

#include <locale>
#include <codecvt>

class __declspec(dllexport) FString
{
public:
	FString() : mWideString(L""), mBasicString("") {};
	~FString() {};

	FString(const WideString& aString)	: mWideString(aString)	 { }
	FString(const wchar_t* aCharPtr)	: mWideString(aCharPtr)  { }
	FString(const BasicString& aString) : mBasicString(aString)	 { }
	FString(const char* aCharPtr)		: mBasicString(aCharPtr) { }

public:
	BasicString& ToBasicString() 
	{
		if (mIsDirty)
			CalculateBasic();

		return mBasicString;
	};
	WideString&	 ToWideString() 
	{
		if (mIsDirty)
			CalculateWide();

		return mWideString;
	};

	const BasicString&	__GetBasic() const { return mBasicString; };
	const WideString&	__GetWide() const { return mWideString; };

private:
	void CalculateBasic()
	{
		size_t size_needed = 0;
		// Get the required size
		wcstombs_s(&size_needed, nullptr, 0, mWideString.Get().c_str(), 0);
		if (size_needed == 0) throw std::runtime_error("Conversion failed");

		mBasicString.Get().resize(size_needed);
		// Convert the string
		wcstombs_s(&size_needed, &mBasicString.Get()[0], mBasicString.Get().size() + 1, mWideString.Get().c_str(), mWideString.Size() + 1);
		mIsDirty = false;
	};

	void CalculateWide()
	{
		size_t size_needed = 0;
		// Get the required size
		mbstowcs_s(&size_needed, nullptr, 0, mBasicString.Get().c_str(), 0);
		if (size_needed == 0) throw std::runtime_error("Conversion failed");

		mWideString.Get().resize(size_needed);
		// Convert the string
		mbstowcs_s(&size_needed, &mWideString.Get()[0], mWideString.Get().size() + 1, mBasicString.Get().c_str(), mBasicString.Size() + 1);
		mIsDirty = false;
	};

public:
#pragma region BASIC_STRING
	void operator=(const BasicString& aOther)
	{
		mIsDirty = true;
		mBasicString = aOther;
	}
	void operator=(const char* aCharPtr)
	{
		mIsDirty = true;
		mBasicString = aCharPtr;
	}

	bool operator==(const BasicString& aOther) const
	{
		return mBasicString == aOther;
	}
	bool operator==(const char* aCharPtr) const
	{
		return mBasicString == aCharPtr;
	}

	void operator+=(const BasicString& aOther)
	{
		mIsDirty = true;
		mBasicString += aOther;
	}
	void operator+=(const char* aCharPtr)
	{
		mIsDirty = true;
		mBasicString += aCharPtr;
	}

	FString& operator+(const BasicString& aOther)
	{
		mIsDirty = true;
		mBasicString += aOther;
		return  *this;
	}
	FString& operator+(const char* aCharPtr)
	{
		mIsDirty = true;
		mBasicString += aCharPtr;
		return  *this;
	}
#pragma endregion

#pragma region WIDE_STRING
	void operator=(const WideString& aOther)
	{
		mIsDirty = true;
		mWideString = aOther;
	}
	void operator=(const wchar_t* aCharPtr)
	{
		mIsDirty = true;
		mWideString = aCharPtr;
	}

	bool operator==(const WideString& aOther) const
	{
		return mWideString == aOther;
	}
	bool operator==(const wchar_t* aCharPtr) const
	{
		return mWideString == aCharPtr;
	}

	void operator+=(const WideString& aOther)
	{
		mIsDirty = true;
		mWideString += aOther;
	}
	void operator+=(const wchar_t* aCharPtr)
	{
		mIsDirty = true;
		mWideString += aCharPtr;
	}

	FString& operator+(const WideString& aOther)
	{
		mIsDirty = true;
		mWideString += aOther;
		return *this;
	}
	FString& operator+(const wchar_t* aCharPtr)
	{
		mIsDirty = true;
		mWideString += aCharPtr;
		return  *this;
	}
#pragma endregion

#pragma region COMBINED_STRING
	void operator=(const FString& aOther)
	{
		this->mBasicString = aOther.mBasicString;
		this->mWideString = aOther.mWideString;
		mIsDirty = true;
	}

	bool operator==(const FString& aOther) const
	{
		return (
			this->mWideString == aOther.mWideString ||
			this->mBasicString == aOther.mBasicString
			);
	}

	void operator+=(const FString& aOther)
	{
		mIsDirty = true;
		this->mWideString += aOther.mWideString;
		this->mBasicString += aOther.mBasicString;
	}

	FString& operator+(const FString& aOther)
	{
		mIsDirty = true;
		this->mWideString += aOther.mWideString;
		this->mBasicString += aOther.mBasicString;
		return *this;
	}

	operator std::string() const {
		return mBasicString.Get();
	}

	operator std::wstring() const {
		return mWideString.Get();
	}

#pragma endregion
private:
	BasicString mBasicString;
	WideString mWideString;

	bool mIsDirty = true;
};

namespace std {
	template <>
	struct hash<FString> {
		std::size_t operator()(const FString& s) const noexcept {
			// Combine the hash values of the underlying BasicString and WideString
			std::size_t hashValue = 0;
			// Use std::hash to hash the BasicString
			hashValue ^= std::hash<BasicString>()(s.__GetBasic()) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
			// Use std::hash to hash the WideString
			hashValue ^= std::hash<WideString>()(s.__GetWide()) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
			return hashValue;
		}
	};
}