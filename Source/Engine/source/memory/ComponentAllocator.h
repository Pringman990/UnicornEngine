#pragma once
#include "reflection/Reflection.h"

class ComponentAllocator
{
public:
	ComponentAllocator();
	ComponentAllocator(const reflection::TypeInfo& aType, uint32_t aGrowthRate = 10);
	~ComponentAllocator();

	void* Allocate();
	void Deallocate(void* aComponent);

private:
	std::vector<char> mData;
	std::vector<size_t> mFreeList;
	
	reflection::TypeInfo mComponentType;
	size_t mNextIndex;
	uint32_t mGrowthRate;
};