#pragma once
#include <Core.h>

class GPUFence
{
public:
	GPUFence();
	~GPUFence();

	/**
	* @param Signaled If false block CPU until GPU work is done
	*/
	static GPUFence* Create(LogicalDevice* Device, bool Signaled = false);

	void Wait(uint64 Timeout = UINT64_MAX) const;
	void Reset();
	bool IsSignaled() const;

public:
	operator VkFence() const noexcept { return mFence; }
private:
	VkFence mFence;
	LogicalDevice* mDevice;
};