#pragma once
#include <EngineMinimal.h>
#include "RendererMinimal.h"

namespace DDS
{
#define DDPF_ALPHAPIXELS 0x1
#define DDPF_ALPHA       0x2
#define DDPF_FOURCC      0x4
#define DDPF_RGB         0x40
#define DDPF_YUV         0x200
#define DDPF_LUMINANCE   0x20000

    struct DDS_PIXELFORMAT
    {
        uint32 size;
        uint32 flags;
        uint32 fourCC;
        uint32 RGBBitCount;
        uint32 RBitMask;
        uint32 GBitMask;
        uint32 BBitMask;
        uint32 ABitMask;
    };

    struct DDS_HEADER
    {
        uint32 size;
        uint32 flags;
        uint32 height;
        uint32 width;
        uint32 pitchOrLinearSize;
        uint32 depth;
        uint32 mipMapCount;
        uint32 reserved1[11];
        DDS_PIXELFORMAT ddspf;
        uint32 caps;
        uint32 caps2;
        uint32 caps3;
        uint32 caps4;
        uint32 reserved2;
    };

    struct DDS_HEADER_DXT10
    {
        DXGI_FORMAT dxgiFormat;
        uint32_t resourceDimension;
        uint32_t miscFlag; // see D3D11_RESOURCE_MISC_FLAG
        uint32_t arraySize;
        uint32_t miscFlags2; // see DDS_MISC_FLAGS2
    };

    constexpr uint32_t DDS_MAGIC = 0x20534444; // "DDS "
    constexpr size_t DDS_MIN_HEADER_SIZE = sizeof(uint32_t) + sizeof(DDS_HEADER);
    constexpr size_t DDS_DX10_HEADER_SIZE = sizeof(uint32_t) + sizeof(DDS_HEADER) + sizeof(DDS_HEADER_DXT10);

    constexpr uint32 MakeFourCC(char c0, char c1, char c2, char c3)
    {
        return ((uint32)c0) | ((uint32)c1 << 8) | ((uint32)c2 << 16) | ((uint32)c3 << 24);
    }

    constexpr const uint32 FOURCC_DXT1 = MakeFourCC('D', 'X', 'T', '1');
    constexpr const uint32 FOURCC_DXT3 = MakeFourCC('D', 'X', 'T', '3');
    constexpr const uint32 FOURCC_DXT5 = MakeFourCC('D', 'X', 'T', '5');
    constexpr const uint32 FOURCC_DX10 = MakeFourCC('D', 'X', '1', '0');
}

enum class DecodedImageType
{
	Image2D,
};

struct ImageDecodeData
{
	DecodedImageType type;
	RenderFormat format;
	uint32 width = 0;
	uint32 height = 0;
	ByteBuffer buffer;

	bool IsValid() const
	{
		return buffer.size() > 0;
	}
};

/**
* Decoder for multiple different image formats.
* 
* @info 
* Supported image formats include: png, jpeg, dds.
*/
class ImageDecoder
{
public:
#undef LoadImage

	/**
	* Loads image data from buffer.
	* ImageDecodeData will be invalid if the buffer could not be read.
	*/
	static ImageDecodeData LoadImage(const ByteBuffer& Buffer, const String& FileExtension);

	static inline constexpr bool IsFormatSupported(const String& Format) { return IsAnyOf(Format, "png", "jpeg", "dds"); }

private:
	static ImageDecodeData DecodeDDS(const ByteBuffer& Buffer);
	static ImageDecodeData DecodePNGJPEG(const ByteBuffer& Buffer);

    static DXGI_FORMAT DDSFormatToDXGI(const DDS::DDS_PIXELFORMAT& ddspf, const DDS::DDS_HEADER_DXT10* dx10Header = nullptr);
};