#pragma once
#include <Core.h>

class GPUBarrier
{
public:

	static void ImageBarrier(
		VkCommandBuffer Cmd,
		VkImage Image,
		VkImageLayout OldLayout,
		VkImageLayout NewLayout,
		VkPipelineStageFlags SrcStage,
		VkPipelineStageFlags DstStage,
		VkAccessFlags SrcAccess,
		VkAccessFlags DstAccess,
		VkImageAspectFlags AspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		uint32_t MipLevels = 1,
		uint32_t LayerCount = 1
	);

	//Prepare swapchain image for rendering
	static void TransitionFromSwapchain(VkCommandBuffer Cmd, GPUTexture* Texture);

	//Prepare swapchain image for presentation
	static void TransitionToSwapchain(VkCommandBuffer Cmd, GPUTexture* Texture);
	
	//Prepare for texture upload
	static void TransitionToTransferDst(VkCommandBuffer Cmd, GPUTexture* Texture);
	
	//Prepare for sampling
	static void TransitionToShaderRead(VkCommandBuffer Cmd, GPUTexture* Texture);
	
	//Prepare for write
	static void TransitionToShaderWrite(VkCommandBuffer Cmd, GPUTexture* Texture);

	//For depth
	static void TransitionToDepthAttachment(VkCommandBuffer Cmd, GPUTexture* Texture);

	//For Compute shader image writing
	static void TransitionToGeneral(VkCommandBuffer Cmd, GPUTexture* Texture);

private:
	GPUBarrier() = delete;
	~GPUBarrier() = delete;
};
