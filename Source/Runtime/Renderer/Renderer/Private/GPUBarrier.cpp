#include "GPUBarrier.h"

void GPUBarrier::ImageBarrier(
    VkCommandBuffer Cmd,
	VkImage Image,
	VkImageLayout OldLayout, 
	VkImageLayout NewLayout,
	VkPipelineStageFlags SrcStage, 
	VkPipelineStageFlags DstStage,
	VkAccessFlags SrcAccess, 
	VkAccessFlags DstAccess, 
	VkImageAspectFlags AspectMask,
	uint32_t MipLevels, 
	uint32_t LayerCount
)
{
	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

	barrier.oldLayout = OldLayout;
	barrier.newLayout = NewLayout;
	
    barrier.image = Image;
	
    barrier.srcAccessMask = SrcAccess;
	barrier.dstAccessMask = DstAccess;
	
    barrier.subresourceRange.aspectMask = AspectMask;
	
    barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.layerCount = LayerCount;
	
	barrier.subresourceRange.levelCount = MipLevels;
	barrier.subresourceRange.baseArrayLayer = 0;

	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	vkCmdPipelineBarrier(
		Cmd,
		SrcStage,
        DstStage,
		0,
		0, 
        nullptr,
		0, 
        nullptr,
		1,
        &barrier
	);
}

void GPUBarrier::TransitionFromSwapchain(VkCommandBuffer Cmd, GPUTexture* Texture)
{
    ImageBarrier(
        Cmd,
        Texture->image,
        Texture->currentImageLayout,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        0,
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        VK_IMAGE_ASPECT_COLOR_BIT,
        1, 
        1
    );

    Texture->currentImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
}

// Color Attachment -> Swapchain (end of frame, before present)
void GPUBarrier::TransitionToSwapchain(VkCommandBuffer Cmd, GPUTexture* Texture)
{
    ImageBarrier(
        Cmd,
        Texture->image,
        Texture->currentImageLayout,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        0,
        VK_IMAGE_ASPECT_COLOR_BIT,
        1, 
        1
    );

    Texture->currentImageLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
}

// Texture Upload: Undefined -> TransferDst
void GPUBarrier::TransitionToTransferDst(VkCommandBuffer Cmd, GPUTexture* Texture)
{
    ImageBarrier(
        Cmd,
        Texture->image,
        Texture->currentImageLayout,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        0,
        VK_ACCESS_TRANSFER_WRITE_BIT
    );

    Texture->currentImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
}

// Upload Done -> Shader Read
void GPUBarrier::TransitionToShaderRead(VkCommandBuffer Cmd, GPUTexture* Texture)
{
    ImageBarrier(
        Cmd,
        Texture->image,
        Texture->currentImageLayout,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_ACCESS_SHADER_READ_BIT
    );

    Texture->currentImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}

void GPUBarrier::TransitionToShaderWrite(VkCommandBuffer Cmd, GPUTexture* Texture)
{
    ImageBarrier(
        Cmd,
        Texture->image,
        Texture->currentImageLayout,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_ACCESS_SHADER_READ_BIT,
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        VK_IMAGE_ASPECT_COLOR_BIT
    );

    Texture->currentImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
}

// Depth Attachment
void GPUBarrier::TransitionToDepthAttachment(VkCommandBuffer Cmd, GPUTexture* Texture)
{
    ImageBarrier(
        Cmd,
        Texture->image,
        Texture->currentImageLayout,
        VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        0,
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        VK_IMAGE_ASPECT_DEPTH_BIT
    );

    Texture->currentImageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
}

void GPUBarrier::TransitionToGeneral(VkCommandBuffer Cmd, GPUTexture* Texture)
{
    ImageBarrier(
        Cmd,
        Texture->image,
        Texture->currentImageLayout,
        VK_IMAGE_LAYOUT_GENERAL,
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,    // source stage depends on previous usage
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,    // destination stage, usually the shader stage that will use it
        VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT, // access masks for read/write in shader
        VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT,
        VK_IMAGE_ASPECT_COLOR_BIT,
        1,
        1
    );

    Texture->currentImageLayout = VK_IMAGE_LAYOUT_GENERAL;
}