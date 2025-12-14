#pragma once
#include <Core.h>
#include "Reflection/ReflectionRegistry.h"

class EComponentAllocator
{
public:
	EComponentAllocator() 
		:
		mType(nullptr),
		mGrowthRate(0),
		mComponentCount(0)
	{
	};

	EComponentAllocator(const refl::Type* Type, size_t InitalReserve, size_t GrowthRate)
		: 
		mType(Type),
		mGrowthRate(GrowthRate * mType->size),
		mComponentCount(0)
	{
		mComponents.resize(InitalReserve * mType->size);
	}

	~EComponentAllocator() 
	{
		for (size_t i = 0; i < mComponentCount; i++)
		{
			if(mType->functions.deconstructor)
				mType->functions.deconstructor(Get(i));
		}
	};

	uint32 GetCount() const { return static_cast<uint32>(mComponentCount); }

	/// <summary>
	/// Allocates a space for a component
	/// </summary>
	/// <returns>
	/// Pointer to start of the allocated memory
	/// </returns>
	void* Allocate()
	{
		GrowIfNeeded();

		void* ptr = Get(mComponentCount);
		mType->functions.constructor(ptr);
		mComponentCount++;
		return ptr;
	}

	void RemoveAt(size_t Index)
	{
		size_t last = mComponentCount - 1;

		if (Index != last)
		{
			void* dst = Get(Index);
			void* src = Get(last);

			//Destory the object
			mType->functions.deconstructor(dst);

			//Move last object to the deleted distination
			mType->functions.moveconstructor(dst, src);

			//Destroy where the moved object was
			mType->functions.deconstructor(src);
		}
		else
		{
			mType->functions.deconstructor(Get(last));
		}

		mComponentCount--;
	}

	void* Get(size_t Index)
	{
		if (Index > mComponentCount)
		{
			return nullptr;
		}

		return mComponents.data() + Index * mType->size;
	}

	/*template<typename T>
	T& Get(size_t Index)
	{
		if (Index * mComponentSize > mBuffer.size() - mComponentSize)
		{
			return nullptr;
		}

		return *reinterpret_cast<T*>(&mBuffer[Index * mComponentSize]);
	}*/

	//template<typename T>
	//const T& Get(size_t Index) const
	//{
	//	if (Index * mComponentSize > mBuffer.size() - mComponentSize)
	//	{
	//		return {};
	//	}

	//	return *reinterpret_cast<const T*>(&mBuffer[Index * mComponentSize]);
	//}

	//template<typename T>
	//T& Back()
	//{
	//	return *reinterpret_cast<T*>(&mBuffer[mComponentCount * mComponentSize]);
	//}

public:
	//struct Iterator
	//{
	//	char* ptr;
	//	size_t stride;

	//	template<typename T>
	//	T& operator*() const
	//	{
	//		return *reinterpret_cast<T*>(ptr);
	//	}

	//	Iterator& operator++()
	//	{
	//		ptr += stride;
	//		return *this;
	//	}

	//	bool operator!=(const Iterator& Other) const
	//	{
	//		return ptr != Other.ptr;
	//	}
	//};

	//Iterator begin()
	//{
	//	return Iterator{mBuffer.data(), mType.size };
	//}

	//Iterator end()
	//{
	//	return Iterator{ mBuffer.data() + mComponentCount * mType.size, mType.size };
	//}

private:
	void GrowIfNeeded()
	{
		if (((mComponentCount + 1) * mType->size) > mComponents.size())
		{
			mComponents.resize(mComponents.size() + mGrowthRate);
		}
	}
private:
	ByteBuffer mComponents;

	const refl::Type* mType;
	size_t mGrowthRate;
	size_t mComponentCount;
};