#include "RenderAssets/Texture2DManager.h"

#include "Renderer.h"
#include "GPUResources/GPUTextureManager.h"
#include "ImageDecoder.h"
#include <Assets/AssetRegistry.h>

Textrue2DManager::Textrue2DManager()
{
}

Textrue2DManager::~Textrue2DManager()
{
}

Texture2D* Textrue2DManager::Load(const String& VirtualPath)
{
	AssetFileReadData readData = AssetRegistry::ReadAssetFile(VirtualPath);

	if (!readData.UUID.IsValid())
	{
		LOG_ERROR("Trying to load asset without uuid");
		return nullptr;
	}

	Texture2D* asset = new Texture2D(readData.UUID);
	asset->SetMetaPath(VirtualPath);
	asset->SetSourcePath(readData.SourcePath);
	asset->SetType(readData.Type);

	ByteBuffer data = FileSystem::Instance()->ReadAll(readData.SourcePath);
	ImageDecodeData decodeData = ImageDecoder::LoadImage(data, ExtractExtension(readData.SourcePath));
	if (!decodeData.IsValid())
	{
		LOG_ERROR("Loading texture asset failed trying to import source");
		delete asset;
		return nullptr;
	}

	GPUResourceHandle<GPUTexture> texture = Renderer::Instance()->GetGPUTextureManager()->CreateTexture(decodeData.buffer, Vector3i(decodeData.width, decodeData.height, 0), decodeData.format, TextureBindFlags::ShaderRead);
	if (!texture)
	{
		LOG_ERROR("Loading texture asset failed trying to import source");
		delete asset;
		return nullptr;
	}
	asset->SetGPUTextureHandle(texture);

	return asset;
}

Texture2D* Textrue2DManager::ImportSource(const String& VirtualSourcePath)
{
	ByteBuffer data = FileSystem::Instance()->ReadAll(VirtualSourcePath);
	ImageDecodeData decodeData = ImageDecoder::LoadImage(data, ExtractExtension(VirtualSourcePath));
	if (!decodeData.IsValid())
	{
		LOG_ERROR("Failed to decode image when importing source, path: {}", VirtualSourcePath);
		return nullptr;
	}

	GPUResourceHandle<GPUTexture> texture = Renderer::Instance()->GetGPUTextureManager()->CreateTexture2D(Vector2i(decodeData.width, decodeData.height), decodeData.format, TextureBindFlags::ShaderRead);

	UniqueID128 uuid = UniqueID128::FromRandom();
	Texture2D* asset = new Texture2D(uuid);
	asset->SetSourcePath(VirtualSourcePath);
	asset->SetGPUTextureHandle(texture);

	String type = ExtractTypeInfoNameWithoutSpecifier(typeid(Texture2D).name());
	asset->SetType(type);

	String assetName = ExtractNameFromPath(VirtualSourcePath);
	String assetPath = ExtractPathWithoutExtension(VirtualSourcePath);
	assetPath += ".asset";
	AssetRegistry::CreateAssetFile(assetPath, type, assetName, uuid, {}, VirtualSourcePath);

	return asset;
}

Texture2D* Textrue2DManager::ReimportSource(const String& VirtualSourcePath, UniqueID128 UUID)
{
	THROW("Not Implemeted");
	return nullptr;
}

bool Textrue2DManager::UnLoad(const String& VirtualPath)
{
	THROW("Not Implemeted");
	return false;
}
