#include "ResourceLoaders/Texture2DLoader.h"
#include <stb_image.h>

#include "TextureFactory.h"
#include "Renderer.h"
#include "GPUBarrier.h"

Texture2DLoader::Texture2DLoader()
{
}

Texture2DLoader::~Texture2DLoader()
{
}

AssetLoadResult<Texture2D> Texture2DLoader::Load(const String& VirtualPath, Texture2D* Asset)
{
	ByteBuffer buffer = FileSystem::ReadAll(VirtualPath);

	int32 width, height, channel;
	//0 = keep original channel, 3 = force RGB, 4 = force RGBA
	byte* rawImage = stbi_load_from_memory(buffer.data(), static_cast<int32>(buffer.size()), &width, &height, &channel, 4);
	if (!rawImage)
	{
		_LOG_RENDERER_WARNING("Couldn't load image: {}", VirtualPath);
		{
			return {};
		}
	}
	int32 imageSize = width * height * 4;

	AssetLoadResult<Texture2D> result;
	result.metadata.name = VirtualPath;
	result.metadata.extent = Vector2(width, height);

	result.resourceData.gpuHandle = Renderer::Get()->GetGPUResourceManager().AllocateResource<GPUTexture>();
	
	ByteBuffer textureData(imageSize);
	memcpy_s(textureData.data(), imageSize, rawImage, imageSize);

	stbi_image_free(rawImage);
	
	auto resourceData = result.resourceData;
	int32 imageSizeCopy = imageSize;

	TextureFactory::TextureCreateInfo textureCreateInfo{};
	textureCreateInfo.pixelData = textureData;
	textureCreateInfo.extent = Vector3(width, height, 1);

	Renderer::Get()->GetGPUResourceManager().UploadTask(
		[resourceData, textureCreateInfo, imageSizeCopy, Asset](CommandBuffer* Cmd)
		{
			auto& resourceManager = Renderer::Get()->GetGPUResourceManager();
			GPUTexture* texture = resourceManager.GetResource<GPUTexture>(resourceData.gpuHandle);
			
			TextureFactory::CreateTexture2D(texture, textureCreateInfo);

			GPUBarrier::TransitionToTransferDst(*Cmd, texture);

			SharedPtr<GenericGPUBuffer> stagingBuffer = GenericGPUBuffer::CreateSingleFrameBuffer(
				imageSizeCopy,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			);

			VkExtent3D extent = { (uint32)textureCreateInfo.extent.x, (uint32)textureCreateInfo.extent.y, 1 };
			TextureFactory::CopyRawDataToTexture(textureCreateInfo.pixelData.data(), extent, Cmd, texture->image, *stagingBuffer);
		
			GPUBarrier::TransitionToShaderRead(*Cmd, texture);
		});

	return result;
}

bool Texture2DLoader::Save(const String& VirtualPath, AssetHandle<Texture2D> Handle)
{
	return false;
}
