#include "RenderAssets/MaterialManager.h"

#include <Assets/AssetRegistry.h>
#include "MaterialDecoder.h"

#include "Renderer.h"
#include "ShaderManager.h"
#include "RenderAssets/Texture2D.h"
#include "GPUResources/GPUMaterialManager.h"

MaterialManager::MaterialManager()
{
}

MaterialManager::~MaterialManager()
{
}

Material* MaterialManager::Load(const String& VirtualPath)
{
	AssetFileReadData readData = AssetRegistry::ReadAssetFile(VirtualPath);

	if (!readData.UUID.IsValid())
	{
		LOG_ERROR("Trying to load asset without uuid");
		return nullptr;
	}

	Material* material = new Material(readData.UUID);
	material->SetMetaPath(VirtualPath);
	material->SetSourcePath(readData.SourcePath);
	material->SetType(readData.Type);

	ByteBuffer materialData = GET_FILESYSTEM()->ReadAll(VirtualPath);
	MaterialDecodeData decodeData = MaterialDecoder::LoadMaterial(materialData);
	if (!decodeData.IsValid())
	{
		LOG_ERROR("Loading mesh asset failed trying to import source");
		delete material;
		return nullptr;
	}

	GPUResourceHandle<ShaderProgram> programHandle = GET_RENDERER()->GetShaderManager()->TryGetShaderProgram(decodeData.shaderProgram);
	material->SetGPUMaterialHandle(GET_RENDERER()->GetGPUMaterialManager()->CreateMaterialFromProgram(programHandle));

	ShaderProgram* program = GET_RENDERER()->GetShaderManager()->GetInternalShaderProgram(programHandle);

	for (auto& resource : program->fs.reflectedInfo.boundResources)
	{
		if (resource.type != ShaderResourceType::Texture)
		{
			continue;
		}

		bool found = false;
		for (uint32 i = 0; i < decodeData.textures.size(); i++)
		{
			if (decodeData.textures[i].second != resource.name)
			{
				continue;
			}

			if (decodeData.textures[i].first.IsValid())
			{
				material->AddMaterialParameter(decodeData.textures[i].second, MaterialParameterType::Texture2D, decodeData.textures[i].first);
				found = true;
			}

			break;
		}

		if (!found)
		{
			material->AddMaterialParameter(resource.name, MaterialParameterType::Undefined, {});
			LOG_WARNING("Tried to get texture2d from decoded material but the texture was not found");
		}
	}

	return material;
}

Material* MaterialManager::ImportSource(const String& VirtualSourcePath)
{
	THROW("MaterialManager does not support any sources");
	return nullptr;
}

Material* MaterialManager::ReimportSource(const String& VirtualSourcePath, UniqueID128 UUID)
{
	return nullptr;
}

bool MaterialManager::UnLoad(const String& VirtualPath)
{
	return false;
}