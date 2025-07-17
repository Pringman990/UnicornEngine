#pragma once
#include <Core.h>

#include "TextureRenderTarget.h"
#include "GPUTexture.h"

class PhysicalDevice;
class LogicalDevice;

class SwapChain
{
public:
	SwapChain();
	~SwapChain();

	static SwapChain* Create(PhysicalDevice* PhysicalDevice, LogicalDevice* Logical, Surface* Surface);
	void ReCreate(PhysicalDevice* PhysicalDevice, LogicalDevice* Logical, Surface* Surface);

	const GPUResourceHandle<GPUTexture> GetRenderTarget(uint32 ImageIndex) const { return mRenderTargets[ImageIndex]; };
	uint32 GetMinSupportedImageCount() const { return mMinImageCountSupported; };
	uint32 GetImageCount() const { return mImageCount; };
	const VkFormat& GetImageFormat() const { return mImageFormat; };
	const VkExtent2D& GetExtent() const { return mExtent; };

	VkSwapchainKHR* GetAdressOf() noexcept { return &mSwapChain; };
	VkSwapchainKHR GetRaw() const { return mSwapChain; };
public:
	operator VkSwapchainKHR() const noexcept { return mSwapChain; }

private:
	void Destroy();

	static void Create(SwapChain* SwapChainPtr, PhysicalDevice* PhysicalDevice, LogicalDevice* Logical, Surface* Surface);

	static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const Vector<VkSurfaceFormatKHR>& AvailableFormats);
	static VkPresentModeKHR ChooseSwapPresentFormat(const Vector<VkPresentModeKHR>& AvailablePresentModes);
	static VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& Capabilites);
private:
	VkSwapchainKHR mSwapChain;

	Vector<GPUResourceHandle<GPUTexture>> mRenderTargets;

	uint32 mMinImageCountSupported;
	uint32 mImageCount;
	VkFormat mImageFormat;
	VkExtent2D mExtent;
};