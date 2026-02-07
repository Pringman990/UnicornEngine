#pragma once
#include <Core.h>
#include "Reflection/ReflectionRegistry.h"

class EComponentAllocator
{
public:
	EComponentAllocator() 
		:
		mComponents(nullptr),
		mType(nullptr),
		mGrowthRate(0),
		mComponentCount(0),
		mCapacity(0),
		mAlignment(0)
	{
	};

	EComponentAllocator(const refl::Type* Type, size_t InitalReserve, size_t GrowthRate)
		: 
		mType(Type),
		mGrowthRate(GrowthRate * mType->size),
		mComponentCount(0)
	{
#undef max
		mAlignment = std::max(mType->alignment, alignof(std::max_align_t));

		mCapacity = InitalReserve;
		mComponents = operator new (mCapacity * mType->size, std::align_val_t(mAlignment));
	}

	~EComponentAllocator() 
	{
		for (size_t i = 0; i < mComponentCount; i++)
			mType->functions.deconstructor(Get(i));
		
		operator delete(mComponents, std::align_val_t(mAlignment));
	};

	EComponentAllocator(EComponentAllocator&& Other) noexcept
		:
		mComponents(Other.mComponents),
		mType(Other.mType),
		mGrowthRate(Other.mGrowthRate),
		mComponentCount(Other.mComponentCount),
		mCapacity(Other.mCapacity),
		mAlignment(Other.mAlignment)
	{
		Other.mComponents = nullptr;
		Other.mType = nullptr;
		Other.mComponentCount = 0;
		Other.mCapacity = 0;
		Other.mAlignment = 0;
		Other.mGrowthRate = 0;
	};

	EComponentAllocator& operator=(EComponentAllocator&& Other) noexcept
	{
		if (this != nullptr)
		{
			for (size_t i = 0; i < mComponentCount; i++)
				mType->functions.deconstructor(Get(i));

			operator delete(mComponents, std::align_val_t(mAlignment));

			mComponents = Other.mComponents;
			mType = Other.mType;
			mCapacity = Other.mCapacity;
			mComponentCount = Other.mComponentCount;
			mAlignment = Other.mAlignment;
			mGrowthRate = Other.mGrowthRate;

			Other.mComponents = nullptr;
			Other.mType = nullptr;
			Other.mComponentCount = 0;
			Other.mCapacity = 0;
			Other.mAlignment = 0;
			Other.mGrowthRate = 0;
		}

		return *this;
	};

	EComponentAllocator(const EComponentAllocator&) = delete;
	EComponentAllocator& operator=(const EComponentAllocator&) = delete;

	uint32 GetCount() const { return static_cast<uint32>(mComponentCount); }

	/// <summary>
	/// Allocates a space for a component
	/// </summary>
	/// <returns>
	/// Pointer to start of the allocated memory
	/// </returns>
	void* Allocate()
	{
		if (mComponentCount >= mCapacity)
			Grow();

		void* ptr = GetUnsafe(mComponentCount);
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

			//Move last object to the deleted destination
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

	/**
	* Will not check if Index is out of bounds.
	*/
	void* GetUnsafe(size_t Index)
	{
		return static_cast<byte*>(mComponents) + (Index * mType->size);
	}

	void* Get(size_t Index)
	{
		if (Index >= mComponentCount) 
			return nullptr;
		return static_cast<byte*>(mComponents) + (Index * mType->size);
	}

	const void* Get(size_t Index) const
	{
		if (Index >= mComponentCount) 
			return nullptr;
		return static_cast<byte*>(mComponents) + (Index * mType->size);
	}

private:
	void Grow()
	{
		size_t newCapacity = mCapacity + mGrowthRate;
		void* newMemory = operator new(newCapacity * mType->size, std::align_val_t(mAlignment));

		for (size_t i = 0; i < mComponentCount; i++)
		{
			void* oldPtr = Get(i);
			void* newPtr = static_cast<byte*>(newMemory) + (i * mType->size);
			mType->functions.moveconstructor(newPtr, oldPtr);
			mType->functions.deconstructor(oldPtr);
		}

		operator delete(mComponents, std::align_val_t(mAlignment));
		mComponents = newMemory;
		mCapacity = newCapacity;
	}
private:
	//ByteBuffer mComponents;
	void* mComponents;

	const refl::Type* mType;
	size_t mGrowthRate;
	size_t mCapacity;
	size_t mComponentCount;
	size_t mAlignment;
};