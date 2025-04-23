#pragma once
#include <Core.h>
#include <Memory/AlignedAllocator.h>

class EComponentAllocator
{
public:
	EComponentAllocator() {};

	EComponentAllocator(ReflectionTypeInfo someTypeInfo, size_t aInitalReserve, size_t aGrowthRate)
		: 
		mGrowthRate(aGrowthRate * someTypeInfo.size), 
		mComponentCount(0),
		mTypeInfo(someTypeInfo)
	{
		mBuffer.reserve(aInitalReserve * mTypeInfo.size);
	}

	~EComponentAllocator() {};

	const ReflectionTypeInfo& GetTypeInfo() const { return mTypeInfo; };

	/// <summary>
	/// Allocates a space for a component
	/// </summary>
	/// <returns>
	/// Pointer to start of the allocated memory
	/// </returns>
	void* Allocate()
	{
		if ((mComponentCount + 1) * mTypeInfo.size > mBuffer.size())
		{
			mBuffer.reserve(mBuffer.size() + mGrowthRate);
		}

		void* ptr = static_cast<void*>(mBuffer.data() + (mComponentCount * mTypeInfo.size));
		mComponentCount++;
		return ptr;
	}

	void Swap(size_t aIndexA, size_t aIndexB)
	{
		//Create temporary storage
		std::vector<char> temp;

		//Allocate enough chars for one component
		temp.resize(mTypeInfo.size);

		//Copy A to temporary memory
		mTypeInfo.move(temp.data(), &mBuffer[aIndexA * mTypeInfo.size]);

		//Move B into where A was
		mTypeInfo.move(&mBuffer[aIndexA * mTypeInfo.size], &mBuffer[aIndexB * mTypeInfo.size]);

		//Move A back to where B was/to the back of the array
		mTypeInfo.move(Back(), temp.data());
	}

	void Pop()
	{
		mBuffer.resize(mBuffer.size() - mTypeInfo.size);
		mComponentCount--;
	}

	void* Get(size_t aIndex)
	{
		return &mBuffer[aIndex * mTypeInfo.size];
	}

	template<typename T>
	T& Get(size_t aIndex)
	{
		return *reinterpret_cast<T*>(&mBuffer[aIndex * mTypeInfo.size]);
	}

	template<typename T>
	const T& Get(size_t aIndex) const
	{
		return *reinterpret_cast<const T*>(&mBuffer[aIndex * mTypeInfo.size]);
	}

	void* Back()
	{
		return &mBuffer[mComponentCount * mTypeInfo.size];
	}

	template<typename T>
	T& Back()
	{
		return *reinterpret_cast<T*>(&mBuffer[mComponentCount * mTypeInfo.size]);
	}

public:
	struct Iterator
	{
		char* ptr;
		size_t stride;

		template<typename T>
		T& operator*() const
		{
			return *reinterpret_cast<T*>(ptr);
		}

		Iterator& operator++()
		{
			ptr += stride;
			return *this;
		}

		bool operator!=(const Iterator& aOther) const
		{
			return ptr != aOther.ptr;
		}
	};

	Iterator begin()
	{
		return Iterator{mBuffer.data(), mTypeInfo.size };
	}

	Iterator end()
	{
		return Iterator{ mBuffer.data() + mComponentCount * mTypeInfo.size, mTypeInfo.size };
	}

private:
	std::vector<char> mBuffer;
	ReflectionTypeInfo mTypeInfo;

	size_t mGrowthRate;
	size_t mComponentCount;
};