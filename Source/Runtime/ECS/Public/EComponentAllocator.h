#pragma once
#include <Core.h>
#include <Memory/AlignedAllocator.h>

class EComponentAllocator
{
public:
//	EComponentAllocator() 
//		:
//		mGrowthRate(0),
//		mComponentCount(0),
//		mType({}) 
//	{
//	};
//
//	EComponentAllocator(const Reflect::TypeSchema& Type, size_t InitalReserve, size_t GrowthRate)
//		: 
//		mGrowthRate(GrowthRate * mType.size),
//		mComponentCount(0),
//		mType(Type)
//	{
//		mBuffer.reserve(InitalReserve * mType.size);
//	}
//
//	~EComponentAllocator() {};
//
//	//const ReflectionTypeInfo& GetTypeInfo() const { return mTypeInfo; };
//
//	/// <summary>
//	/// Allocates a space for a component
//	/// </summary>
//	/// <returns>
//	/// Pointer to start of the allocated memory
//	/// </returns>
//	void* Allocate()
//	{
//		if ((mComponentCount + 1) * mType.size > mBuffer.size())
//		{
//			mBuffer.reserve(mBuffer.size() + mGrowthRate);
//		}
//
//		void* ptr = static_cast<void*>(mBuffer.data() + (mComponentCount * mType.size));
//		mComponentCount++;
//		return ptr;
//	}
//
//	void Swap(size_t IndexA, size_t IndexB)
//	{
//		//Create temporary storage
//		std::vector<char> temp;
//
//		//Allocate enough chars for one component
//		temp.resize(mType.size);
//
//		//Copy A to temporary memory
//		mType.moveFunc(temp.data(), &mBuffer[IndexA * mType.size]);
//
//		//Move B into where A was
//		mType.moveFunc(&mBuffer[IndexA * mType.size], &mBuffer[IndexB * mType.size]);
//
//		//Move A back to where B was/to the back of the array
//		mType.moveFunc(Back(), temp.data());
//	}
//
//	void Pop()
//	{
//		mBuffer.resize(mBuffer.size() - mType.size);
//		mComponentCount--;
//	}
//
//	void* Get(size_t Index)
//	{
//		return &mBuffer[Index * mType.size];
//	}
//
//	template<typename T>
//	T& Get(size_t Index)
//	{
//		return *reinterpret_cast<T*>(&mBuffer[Index * mType.size]);
//	}
//
//	template<typename T>
//	const T& Get(size_t Index) const
//	{
//		return *reinterpret_cast<const T*>(&mBuffer[Index * mType.size]);
//	}
//
//	void* Back()
//	{
//		return &mBuffer[mComponentCount * mType.size];
//	}
//
//	template<typename T>
//	T& Back()
//	{
//		return *reinterpret_cast<T*>(&mBuffer[mComponentCount * mType.size]);
//	}
//
//public:
//	struct Iterator
//	{
//		char* ptr;
//		size_t stride;
//
//		template<typename T>
//		T& operator*() const
//		{
//			return *reinterpret_cast<T*>(ptr);
//		}
//
//		Iterator& operator++()
//		{
//			ptr += stride;
//			return *this;
//		}
//
//		bool operator!=(const Iterator& Other) const
//		{
//			return ptr != Other.ptr;
//		}
//	};
//
//	Iterator begin()
//	{
//		return Iterator{mBuffer.data(), mType.size };
//	}
//
//	Iterator end()
//	{
//		return Iterator{ mBuffer.data() + mComponentCount * mType.size, mType.size };
//	}
//
//private:
//	Vector<char> mBuffer;
//	Reflect::TypeSchema mType;
//
//	size_t mGrowthRate;
//	size_t mComponentCount;
};