#pragma once
#include <unordered_map>
#include "TUniquePtr.h"

template<typename _Key, typename _Val>
class __declspec(dllexport) TUMap
{
public:
	TUMap() : mImpl(MakeUnique<Impl>()) {};
	~TUMap() = default;

	void Insert(const _Key& aKey, const _Val& aValue)
	{
		mImpl->map[aKey] = aValue;
	}

	void Insert(_Key&& aKey, _Val&& aValue)
	{
		mImpl->map[std::move(aKey)] = std::move(aValue);
	}

	void Remove(const _Key& aKey)
	{
		mImpl->map.erase(aKey);
	}

	size_t Size() const
	{
		return mImpl->map.size();
	}

public:
	class Iterator {
	public:
		Iterator(typename std::unordered_map<_Key, _Val>::iterator iter, typename std::unordered_map<_Key, _Val>::iterator end)
			: currentIter(iter), endIter(end) {}

		bool IsValid() const {
			return currentIter != endIter;
		}

		void Next() {
			++currentIter;
		}

		_Key& Key() {
			return currentIter->first;
		}

		_Val& Value() {
			return currentIter->second;
		}

		bool operator!=(const Iterator& aOther) const {
			return this->currentIter != aOther.currentIter;
		}

		bool operator==(const Iterator& aOther) const {
			return this->currentIter == aOther.currentIter;
		}

		void operator++()
		{
			++currentIter;
		}

	private:
		typename std::unordered_map<_Key, _Val>::iterator currentIter;
		typename std::unordered_map<_Key, _Val>::iterator endIter;
	};

	class ConstIterator {
	public:
		ConstIterator(typename std::unordered_map<_Key, _Val>::const_iterator iter, typename std::unordered_map<_Key, _Val>::const_iterator end)
			: currentIter(iter), endIter(end) {}

		bool IsValid() const {
			return currentIter != endIter;
		}

		void Next() {
			++currentIter;
		}

		const _Key& Key() const {
			return currentIter->first;
		}

		const _Val& Value() const {
			return currentIter->second;
		}

		bool operator!=(const ConstIterator& aOther) const {
			return this->currentIter != aOther.currentIter;
		}

		bool operator==(const ConstIterator& aOther) const {
			return this->currentIter == aOther.currentIter;
		}

		void operator++()
		{
			++currentIter;
		}

	private:
		typename std::unordered_map<_Key, _Val>::const_iterator currentIter;
		typename std::unordered_map<_Key, _Val>::const_iterator endIter;
	};

	Iterator Find(const _Key& aKey) {
		return Iterator(mImpl->map.find(aKey), mImpl->map.end());
	}

	// Const Find: Does not allow modification of the value
	ConstIterator Find(const _Key& aKey) const {
		return ConstIterator(mImpl->map.find(aKey), mImpl->map.end());
	}

	// Begin and End are small letters because for loops need small letters
	Iterator begin() {
		return Iterator(mImpl->map.begin(), mImpl->map.end());
	}

	Iterator end() {
		return Iterator(mImpl->map.end(), mImpl->map.end());
	}

	// Begin and End are small letters because for loops need small letters
	ConstIterator begin() const {
		return ConstIterator(mImpl->map.cbegin(), mImpl->map.cend());
	}

	ConstIterator end() const {
		return ConstIterator(mImpl->map.cend(), mImpl->map.cend());
	}


public:
	_Val& operator[](const _Key& aKey)
	{
		return mImpl->map[aKey];
	}

private:
	struct Impl {
		std::unordered_map<_Key, _Val> map;
	};
	TUniquePtr<Impl> mImpl;
};