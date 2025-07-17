#pragma once
#include <Core.h>

class Semaphore
{
public:
	Semaphore();
	~Semaphore();

	static Semaphore* Create(LogicalDevice* Device);
	
public:
	operator VkSemaphore() const noexcept { return mSemaphore; }
private:
	VkSemaphore mSemaphore;
	LogicalDevice* mDevice;
};