#pragma once
#include <Core.h>

//IMPORTANT: This is a temporary class until the reflection system has been made.
//TODO: remove when reflection system is done
class IComponentAllocator
{
public:
	IComponentAllocator() = default;
	virtual ~IComponentAllocator() = default;

	virtual void* Allocate() = 0;
	virtual void Swap(uint32 From, uint32 To) = 0;
	virtual void Pop() = 0;

	virtual void* Get(uint32 Index) = 0;
	virtual void* Back() = 0;

	virtual uint32 GetCount() const = 0;

private:

};