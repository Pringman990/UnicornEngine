#include "EnginePch.h"
#include "ComponentAllocator.h"

ComponentAllocator::ComponentAllocator()
	:
	mNextIndex(0),
	mGrowthRate(10)
{
}

ComponentAllocator::ComponentAllocator(const reflection::TypeInfo& aType, uint32_t aGrowthRate)
	:
	mComponentType(aType),
	mNextIndex(0),
	mGrowthRate(aGrowthRate)
{
	mData.resize(mComponentType.size * mGrowthRate);
}

ComponentAllocator::~ComponentAllocator()
{
}

void* ComponentAllocator::Allocate()
{
	if (mFreeList.empty())
	{
		size_t offset = mNextIndex;
		mNextIndex++;
		if (offset * mComponentType.size >= mData.size())
		{
			mData.resize(mData.size() + (mComponentType.size * mGrowthRate));
		}
		return mData.data() + (offset * mComponentType.size);
	}
	else
	{
		size_t index = mFreeList.back();
		mFreeList.pop_back();
		return mData.data() + (index * mComponentType.size);
	}
}

void ComponentAllocator::Deallocate(void* aComponent)
{
	size_t index = (static_cast<char*>(aComponent) - mData.data()) / mComponentType.size;
	mFreeList.push_back(index);
}
