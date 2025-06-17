#include "Texture2DLoader.h"
#include <stb_image.h>

#include "Renderer.h"

Texture2DLoader::Texture2DLoader()
{
}

Texture2DLoader::~Texture2DLoader()
{
}

Optional<Texture2D> Texture2DLoader::Load(const String& VirtualPath)
{
	ByteBuffer buffer = FileSystem::ReadAll(VirtualPath);

	int width, height, channel;
	//0 = keep original channel, 3 = force RGB, 4 = force RGBA
	byte* rawImage = stbi_load_from_memory(buffer.data(), static_cast<int32>(buffer.size()), &width, &height, &channel, 4);

	return Renderer::Get()->GetRHI()->GetTextureFactory()->CreateTexture2D(buffer, width, height);
}

bool Texture2DLoader::Save(const String& VirtualPath, SharedPtr<Texture2D> Data)
{
	return false;
}
