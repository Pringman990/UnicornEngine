#pragma once
#include <Core.h>

class Surface
{
public:
	Surface();
	~Surface();

	static Surface* Create(VkInstance Instance);
	static Surface* Create(void* Hwnd, VkInstance Instance);

	VkSurfaceKHR GetRaw() const { return mSurface; };

public:
	operator VkSurfaceKHR() const noexcept { return mSurface; }

private:
	VkSurfaceKHR mSurface;
};