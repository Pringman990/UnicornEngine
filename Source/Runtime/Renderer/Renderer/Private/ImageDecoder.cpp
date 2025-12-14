#include "ImageDecoder.h"

#include <stb_image.h>
#include <LoaderHelpers.h>

ImageDecodeData ImageDecoder::LoadImage(const ByteBuffer& Buffer, const String& FileExtension)
{
	if (Buffer.size() == 0)
		return { };

	if (FileExtension == "dds")
	{
		return DecodeDDS(Buffer);
	}
	else if (FileExtension == "png" || FileExtension == "jpeg")
	{
		return DecodePNGJPEG(Buffer);
	}

	return {};
}

ImageDecodeData ImageDecoder::DecodeDDS(const ByteBuffer& Buffer)
{
	// DDS files always start with the same magic number ("DDS ")
	const auto dwMagicNumber = *reinterpret_cast<const uint32_t*>(Buffer.data());
	if (dwMagicNumber != DDS::DDS_MAGIC)
	{
		return {};
	}

	const DDS::DDS_HEADER* header = reinterpret_cast<const DDS::DDS_HEADER*>(Buffer.data() + sizeof(uint32_t));
	if (header->size != sizeof(DDS::DDS_HEADER) ||
		header->ddspf.size != sizeof(DDS::DDS_PIXELFORMAT))
	{
		return {};
	}

	const DDS::DDS_HEADER_DXT10* dxt10Header = nullptr;
	if ((header->ddspf.flags & DDS_FOURCC) &&
		(MAKEFOURCC('D', 'X', '1', '0') == header->ddspf.fourCC))
	{
		if (Buffer.size() < DDS::DDS_DX10_HEADER_SIZE)
		{
			return {};
		}

		dxt10Header = reinterpret_cast<const DDS::DDS_HEADER_DXT10*>(Buffer.data() + sizeof(uint32_t) + sizeof(DDS::DDS_HEADER));
	}

	size_t offset = DDS::DDS_MIN_HEADER_SIZE + (dxt10Header ? sizeof(DDS::DDS_HEADER_DXT10) : 0u);
	const uint8_t* bitData = Buffer.data() + offset;
	size_t bitSize = Buffer.size() - offset;


	ImageDecodeData data;
	data.format = FromDXFormat(ImageDecoder::DDSFormatToDXGI(header->ddspf, dxt10Header));
	data.type = DecodedImageType::Image2D;
	data.width = header->width;
	data.height = header->height;
	data.buffer.resize(bitSize);
	memcpy_s(data.buffer.data(), bitSize, bitData, bitSize);

	return data;
}

ImageDecodeData ImageDecoder::DecodePNGJPEG(const ByteBuffer& Buffer)
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

DXGI_FORMAT ImageDecoder::DDSFormatToDXGI(const DDS::DDS_PIXELFORMAT& ddspf, const DDS::DDS_HEADER_DXT10* dx10Header)
{
	if (ddspf.flags & DDPF_FOURCC)
	{
		switch (ddspf.fourCC)
		{
		case DDS::FOURCC_DXT1: return DXGI_FORMAT_BC1_UNORM;
		case DDS::FOURCC_DXT3: return DXGI_FORMAT_BC2_UNORM;
		case DDS::FOURCC_DXT5: return DXGI_FORMAT_BC3_UNORM;
		case DDS::FOURCC_DX10:
			if (dx10Header)
			{
				return dx10Header->dxgiFormat; // Use DX10 extended format
			}
			return DXGI_FORMAT_UNKNOWN;
		default: return DXGI_FORMAT_UNKNOWN;
		}
	}

	if (ddspf.flags & DDPF_RGB)
	{
		switch (ddspf.RGBBitCount)
		{
		case 32:
			if (ddspf.RBitMask == 0x000000ff && ddspf.GBitMask == 0x0000ff00 &&
				ddspf.BBitMask == 0x00ff0000 && ddspf.ABitMask == 0xff000000)
				return DXGI_FORMAT_R8G8B8A8_UNORM;
			if (ddspf.RBitMask == 0x00ff0000 && ddspf.GBitMask == 0x0000ff00 &&
				ddspf.BBitMask == 0x000000ff && ddspf.ABitMask == 0xff000000)
				return DXGI_FORMAT_B8G8R8A8_UNORM;
			break;

		case 24:
			if (ddspf.RBitMask == 0x00ff0000 && ddspf.GBitMask == 0x0000ff00 &&
				ddspf.BBitMask == 0x000000ff)
				return DXGI_FORMAT_R8G8B8A8_UNORM; // can pad alpha later
			break;

		case 16:
			if (ddspf.RBitMask == 0x7C00 && ddspf.GBitMask == 0x03E0 &&
				ddspf.BBitMask == 0x001F && ddspf.ABitMask == 0x8000)
				return DXGI_FORMAT_B5G5R5A1_UNORM;
			if (ddspf.RBitMask == 0xF800 && ddspf.GBitMask == 0x07E0 &&
				ddspf.BBitMask == 0x001F)
				return DXGI_FORMAT_B5G6R5_UNORM;
			break;
		}
	}

	if (ddspf.flags & DDPF_LUMINANCE)
	{
		switch (ddspf.RGBBitCount)
		{
		case 8:  return DXGI_FORMAT_R8_UNORM;
		case 16: return DXGI_FORMAT_R16_UNORM;
		}
	}

	if (ddspf.flags & DDPF_ALPHA)
	{
		switch (ddspf.RGBBitCount)
		{
		case 8: return DXGI_FORMAT_A8_UNORM;
		}
	}

	return DXGI_FORMAT_UNKNOWN;
}