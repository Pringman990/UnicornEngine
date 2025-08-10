#pragma once
#include <Core.h>
#include "IGPUResourceFactory.h"

#include "Texture2D.h"
#include "TextureRenderTarget.h"
#include "GenericGPUBuffer.h"

class TextureFactory : public IGPUResourceFactory
{
public:
	struct TextureCreateInfo
	{
		ByteBuffer pixelData;
		Vector3 extent;
	};
public:

	static void CreateTexture2D(GPUTexture* Texture, const TextureCreateInfo& CreateInfo, VkFormat Format = VK_FORMAT_R8G8B8A8_UNORM);
	static GPUResourceHandle<TextureRenderTarget> CreateTextureRenderTarget(VkExtent2D Extent, VkFormat Format, const String& DebugName = "NO_NAME");
	static const GPUResourceHandle<GPUTexture> CreateTextureRenderTargetSC(LogicalDevice& Device, VkImage Image, VkFormat Format, VkExtent2D Extent);

	static void CopyRawDataToTexture(const byte* Data, VkExtent3D Extent, CommandBuffer* Cmd, VkImage Image, GenericGPUBuffer& Buffer);
private:
	TextureFactory() = default;
	~TextureFactory() override = default;

	static void CreateTexture(GPUTexture* Texture, VkExtent3D Extent, VkFormat Format, VkImageUsageFlags Usage);
private:


};