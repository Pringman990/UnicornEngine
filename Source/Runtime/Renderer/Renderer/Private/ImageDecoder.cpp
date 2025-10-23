#include "ImageDecoder.h"

#include <stb_image.h>

ImageDecodeData ImageDecoder::LoadImage(const ByteBuffer& Buffer)
{
	int32 width, height, channel;
	//0 = keep original channel, 3 = force RGB, 4 = force RGBA
	byte* rawImage = stbi_load_from_memory(Buffer.data(), static_cast<int32>(Buffer.size()), &width, &height, &channel, 4);
	if (!rawImage)
	{
		LOG_WARNING("Couldn't load image from memory");
		{
			return {};
		}
	}

	int32 dataSize = width * height * channel;

	ImageDecodeData data;
	data.format = RenderFormat::R8G8B8A8_UNORM;
	data.type = DecodedImageType::Image2D;
	data.width = width;
	data.height = height;
	data.buffer.resize(dataSize);
	memcpy_s(data.buffer.data(), dataSize, rawImage, dataSize);

	stbi_image_free(rawImage);

	return data;
}
