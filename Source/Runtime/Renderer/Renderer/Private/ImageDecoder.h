#pragma once
#include <EngineMinimal.h>
#include "RendererMinimal.h"

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
* Supported image formats include: png, jpeg.
*/
class ImageDecoder
{
public:
#undef LoadImage

	/**
	* Loads image data from buffer.
	* ImageDecodeData will be invalid if the buffer could not be read.
	*/
	static ImageDecodeData LoadImage(const ByteBuffer& Buffer);

	static inline constexpr bool IsFormatSupported(const String& Format) { return IsAnyOf(Format, "png", "jpeg"); }

private:

};