#pragma once
#include <EngineMinimal.h>
#include "RendererMinimal.h"

struct MaterialDecodeData
{
	Vector<std::pair<UniqueID128, String>> textures;
	Path shaderProgram;

	bool IsValid() const
	{
		return !shaderProgram.empty();
	}
};

class MaterialDecoder
{
public:

	static MaterialDecodeData LoadMaterial(const ByteBuffer& Buffer);

	//static inline constexpr bool IsFormatSupported(const String& Format) { return IsAnyOf(Format, "fbx"); }

private:

};