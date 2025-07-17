#include "TextureFactory.h"

#include "GPUBarrier.h"

void TextureFactory::CreateTexture2D(GPUTexture* Texture, const Texture2D::StagingData& StagingData, VkFormat Format)
{
	VkExtent3D extent = {(uint32)StagingData.extent.x, (uint32)StagingData.extent.y, 1};
	CreateTexture(Texture, extent, Format, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);

	////TODO: Fix when multi threading
	//CommandBuffer* commandBuffer = Renderer::Get()->GetCurrentFrameSyncCommandPool()->AllocateCommandBuffer();
	//commandBuffer->Begin();

	//GPUBarrier::TransitionToTransferDst(*commandBuffer, texture->mImage, texture->mCurrentImageLayout);

	//VkDeviceSize imageSize = Extent.width * Extent.height * 1 * 4;

	//GenericGPUBuffer stagingBuffer = GenericGPUBuffer::CreateOneTimeBuffer(
	//	imageSize,
	//	VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	//	VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	//);

	//CopyRawDataToTexture(RawTextureData, VkExtent3D({ .width = Extent.width, .height = Extent.height, .depth = 1 }), commandBuffer, texture->mImage, stagingBuffer);

	//GPUBarrier::TransitionToShaderRead(*commandBuffer, texture->mImage, texture->mCurrentImageLayout);

	//commandBuffer->EndWithSubmit();
	//Renderer::Get()->GetCurrentFrameSyncCommandPool()->FreeCommandBuffer(commandBuffer);

	//auto* resourcePool = Renderer::Get()->GetTexture2DResourcePool();
	//auto resourcehandle = resourcePool->Add(SharedPtr<Texture2D>(texture));

	//texture->mHandle = resourcehandle;
}

GPUResourceHandle<TextureRenderTarget> TextureFactory::CreateTextureRenderTarget(VkExtent2D Extent, VkFormat Format, const String& DebugName)
{
	//TextureCreateData textureData = CreateTexture(Extent, Format, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

	//VkExtent3D extent = { (uint32)StagingData.extent.x, (uint32)StagingData.extent.y, 1 };
	//CreateTexture(Texture, extent, Format, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

    TextureRenderTarget* texture = new TextureRenderTarget();
	//texture->mFormat = Format;
	//texture->mExtent = Extent;
	//texture->mOwnsImage = true;
	//texture->mImage = Texture->image;
	//texture->mImageView = Texture->imageView;
	//texture->mMemory = Texture->memory;

	//texture->FillMetadata(DebugName);

	//auto* resourcePool = Renderer::Get()->GetTextureRenderTargetResourcePool();
	//auto resourcehandle = resourcePool->Add(SharedPtr<TextureRenderTarget>(texture));

	//texture->mHandle = resourcehandle;

    return GPUResourceHandle<TextureRenderTarget>::Invalid();
}

const GPUResourceHandle<GPUTexture> TextureFactory::CreateTextureRenderTargetSC(
	LogicalDevice& Device,
	VkImage Image, 
	VkFormat Format, 
	VkExtent2D Extent
)
{
	const GPUResourceHandle<GPUTexture> handle = Renderer::Get()->GetGPUResourceManager().AllocateResource<GPUTexture>();
	
	GPUTexture* texture = Renderer::Get()->GetGPUResourceManager().GetResource(handle);
	texture->image = Image;
	texture->format = Format;
	texture->memory = nullptr;
	texture->extent = { (uint32)Extent.width, (uint32)Extent.height, 1 };

	VkImageViewCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.image = Image;

	createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	createInfo.format = Format;

	createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

	createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.levelCount = 1;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.layerCount = 1;

	VkResult result = vkCreateImageView(Device, &createInfo, nullptr, &texture->imageView);
	if (result != VK_SUCCESS)
	{
		_THROW_RENDERER("Failed to create rendertarget");
		return GPUResourceHandle<GPUTexture>::Invalid();
	}

	return handle;
}

void TextureFactory::CreateTexture(GPUTexture* Texture, VkExtent3D Extent, VkFormat Format, VkImageUsageFlags Usage)
{
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.format = Format;
	imageInfo.extent.width = Extent.width;
	imageInfo.extent.height = Extent.height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.usage = Usage;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	Texture->currentImageLayout = imageInfo.initialLayout;

	VkDevice device = Renderer::Get()->GetDevice()->GetRaw();

	VkResult result = vkCreateImage(device, &imageInfo, nullptr, &Texture->image);
	if (result != VK_SUCCESS)
	{
		_THROW_RENDERER("Failed to create image");
		return;
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device, Texture->image, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;

	// Find proper memory type index for device local memory
	allocInfo.memoryTypeIndex = Renderer::Get()->GetPhysicalDevice()->FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	result = vkAllocateMemory(device, &allocInfo, nullptr, &Texture->memory);
	if (result != VK_SUCCESS)
	{
		_THROW_RENDERER("Failed to allocate image memory");
		vkDestroyImage(device, Texture->image, nullptr);
		return;
	}
	vkBindImageMemory(device, Texture->image, Texture->memory, 0);

	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = Texture->image;

	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = Format;

	viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView vkView = VK_NULL_HANDLE;
	result = vkCreateImageView(device, &viewInfo, nullptr, &Texture->imageView);
	if (result != VK_SUCCESS)
	{
		_THROW_RENDERER("Failed to create image view");
		vkDestroyImage(device, Texture->image, nullptr);
		vkFreeMemory(device, Texture->memory, nullptr);
		return;
	}
}

void TextureFactory::CopyRawDataToTexture(const byte* Data, VkExtent3D Extent, CommandBuffer* Cmd, VkImage Image, GenericGPUBuffer& Buffer)
{
	VkDeviceSize imageSize = Extent.width * Extent.height * Extent.depth * 4; //4 For RGBA 

	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = { 0,0,0 };
	region.imageExtent = Extent;

	Buffer.Map(Data, imageSize);

	vkCmdCopyBufferToImage(*Cmd, Buffer.GetRaw(), Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}
