#pragma once
#include <Core.h>
#include <Memory/AlignedAllocator.h>

class EComponentAllocator
{
public:
	EComponentAllocator() {};

	EComponentAllocator(ReflectionTypeInfo TypeInfo, size_t InitalReserve, size_t GrowthRate)
		: 
		mGrowthRate(GrowthRate * TypeInfo.size), 
		mComponentCount(0),
		mTypeInfo(TypeInfo)
	{
		mBuffer.reserve(InitalReserve * mTypeInfo.size);
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

	void Swap(size_t IndexA, size_t IndexB)
	{
		//Create temporary storage
		std::vector<char> temp;

		//Allocate enough chars for one component
		temp.resize(mTypeInfo.size);

		//Copy A to temporary memory
		mTypeInfo.move(temp.data(), &mBuffer[IndexA * mTypeInfo.size]);

		//Move B into where A was
		mTypeInfo.move(&mBuffer[IndexA * mTypeInfo.size], &mBuffer[IndexB * mTypeInfo.size]);

		//Move A back to where B was/to the back of the array
		mTypeInfo.move(Back(), temp.data());
	}

	void Pop()
	{
		mBuffer.resize(mBuffer.size() - mTypeInfo.size);
		mComponentCount--;
	}

	void* Get(size_t Index)
	{
		return &mBuffer[Index * mTypeInfo.size];
	}

	template<typename T>
	T& Get(size_t Index)
	{
		return *reinterpret_cast<T*>(&mBuffer[Index * mTypeInfo.size]);
	}

	template<typename T>
	const T& Get(size_t Index) const
	{
		return *reinterpret_cast<const T*>(&mBuffer[Index * mTypeInfo.size]);
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

		bool operator!=(const Iterator& Other) const
		{
			return ptr != Other.ptr;
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
	Vector<char> mBuffer;
	ReflectionTypeInfo mTypeInfo;

	size_t mGrowthRate;
	size_t mComponentCount;
};