#include "GPUResources/GPUMaterialManager.h"

#include "Renderer.h"
#include "ShaderManager.h"
#include "ImageDecoder.h"
#include "RenderAssets/Material.h"
#include "RenderAssets/Texture2D.h"
#include <Assets/AssetRegistry.h>
#include "GPUResources/GPUTextureManager.h"

GPUMaterialManager::GPUMaterialManager(Renderer* InRenderer)
	:
	mRenderer(InRenderer)
{
}

GPUMaterialManager::~GPUMaterialManager()
{
}

void GPUMaterialManager::Init()
{
	auto texMan = mRenderer->GetGPUTextureManager();

	{
		auto shaderHandle = mRenderer->GetShaderManager()->TryGetShaderProgram("shaders://Invalid.hlsl");
		ASSERT(shaderHandle, "Failed to create Standard material");
		mInvalid = CreateMaterialFromProgram(shaderHandle);
	}

	{
		auto shaderHandle = mRenderer->GetShaderManager()->TryGetShaderProgram("shaders://StandardPBR.hlsl");
		ASSERT(shaderHandle, "Failed to create Standard material");

		GPUResourceHandle<GPUMaterial> materialHandle = CreateMaterialFromProgram(shaderHandle);
		mEngineMaterials["StandardPBR"] = materialHandle;

		ByteBuffer imageData = FileSystem::Instance()->ReadAll("engine://Textures/defaultMaterial_c.dds");
		auto imageDecodeData = ImageDecoder::LoadImage(imageData, ExtractExtension("engine://Textures/defaultMaterial_c.dds"));
		auto albedoTexHandle = texMan->CreateTexture(imageDecodeData.buffer, Vector3i(imageDecodeData.width, imageDecodeData.height, 0), imageDecodeData.format, TextureBindFlags::ShaderRead);

		ByteBuffer imageDataN = FileSystem::Instance()->ReadAll("engine://Textures/defaultMaterial_n.dds");
		auto imageDecodeDataN = ImageDecoder::LoadImage(imageDataN, ExtractExtension("engine://Textures/defaultMaterial_n.dds"));
		auto normalTexHandle = texMan->CreateTexture(imageDecodeDataN.buffer, Vector3i(imageDecodeDataN.width, imageDecodeDataN.height, 0), imageDecodeDataN.format, TextureBindFlags::ShaderRead);

		auto mat = GetInternalMaterial(materialHandle);
		mat->boundTextures.push_back({ 0, albedoTexHandle, ShaderStageBind::FS });
		mat->boundTextures.push_back({ 1, normalTexHandle, ShaderStageBind::FS });
	}

	//{
	//	auto shaderHandle = mRenderer->GetShaderManager()->CreateShader("shaders://Mesh_VS.hlsl", "shaders://Textured_Mesh_FS.hlsl");
	//	AssetHandle<Material> materialHandle = CreateMaterial(shaderHandle);
	//	mEngineMaterials["textured_mesh"] = materialHandle;
	//}
}

GPUResourceHandle<GPUMaterial> GPUMaterialManager::CreateMaterialFromProgram(GPUResourceHandle<ShaderProgram> ShaderHandle)
{
	GPUResourceHandle<GPUMaterial> materialHandle = mMaterialPool.Allocate();

	GPUMaterial* material = mMaterialPool.Get(materialHandle);
	material->shaderProgram = ShaderHandle;

	////Make sure we have a clean map before adding.
	//material->shaderParameters.clear();

	//ShaderProgram* program = mRenderer->GetShaderManager()->GetInternalShaderProgram(ShaderHandle);

	//for(const auto& resource : program->vs.reflectedInfo.boundResources)
	//{
	//	material->shaderParameters.insert({ resource.name, {} });
	//}

	//for (const auto& resource : program->fs.reflectedInfo.boundResources)
	//{
	//	MaterialParameter param;
	//	param.bindPoint = resource.bindPoint;
	//	param.value = {};
	//	param.bind = ShaderStageBind::FS;

	//	switch (resource.type)
	//	{
	//	case ShaderResourceType::Texture:
	//		param.type = MaterialParameterType::Texture;
	//		break;
	//	default:
	//		LOG_WARNING("Unsupported material parameter");
	//		continue;
	//	}

	//	material->shaderParameters.insert({ resource.name, param });
	//}

	return materialHandle;
}

GPUResourceHandle<GPUMaterial> GPUMaterialManager::GetEngineMaterial(const String& Name)
{
	auto it = mEngineMaterials.find(Name);
	if (it != mEngineMaterials.end())
	{
		return it->second;
	}

	LOG_WARNING("No engine material with name: {}", Name);
	return GPUResourceHandle<GPUMaterial>::Invalid();
}

void GPUMaterialManager::UpdateFromAsset(GPUResourceHandle<GPUMaterial> GPU, Material* CPU)
{
	GPUMaterial* gpuMat = GetInternalMaterial(GPU);
	if (!gpuMat)
	{
		LOG_WARNING("Failed to update gpu material from cpu, gpu did not exist");
		return;
	}

	gpuMat->boundTextures.clear();

	ShaderProgram* program = mRenderer->GetShaderManager()->GetInternalShaderProgram(gpuMat->shaderProgram);

	//IMPORTANT: If this bool is never set the renderer will try to sync every frame until all is resolved
	bool allResolved = true;

	const auto& cpuParams = CPU->GetShaderParams();
	for (auto& resource : program->fs.reflectedInfo.boundResources)
	{
		auto it = cpuParams.find(resource.name);
		if (it == cpuParams.end())
		{
			continue;
		}

		GPUMaterial::TextureDesc desc{};
		GPUResourceHandle<GPUTexture> gpuTexture = GPUResourceHandle<GPUTexture>::Invalid();
		if (it->second.type == MaterialParameterType::Texture2D)
		{
			UniqueID128 texture2DUUID = std::get<UniqueID128>(it->second.value);
			if (!texture2DUUID.IsValid())
			{
				LOG_WARNING("Tried to get texture2d for cpu-gpu material sync but uuid was invalid");
			}
			else
			{
				AssetRef<Texture2D> texture = AssetRegistry::Instance()->Load<Texture2D>(texture2DUUID);
				if (!texture)
				{
					allResolved = false;
				}

				gpuTexture = texture->GetGPUTextureHandle();
			}
		}

		desc.slot = resource.bindPoint;
		desc.stage = ShaderStageBind::FS;
		desc.texture = gpuTexture;

		gpuMat->boundTextures.push_back(desc);
	}

	for (auto& resource : program->vs.reflectedInfo.boundResources)
	{
		auto it = cpuParams.find(resource.name);
		if (it == cpuParams.end() || it->second.type != MaterialParameterType::Texture2D)
		{
			continue;
		}

		UniqueID128 texture2DUUID = std::get<UniqueID128>(it->second.value);
		if (!texture2DUUID.IsValid())
		{
			LOG_WARNING("Tried to get texture2d for cpu-gpu material sync but uuid was invalid");
			continue;
		}
		AssetRef<Texture2D> texture = AssetRegistry::Instance()->Load<Texture2D>(texture2DUUID);
		if (!texture)
		{
			allResolved = false;
			continue;
		}

		GPUMaterial::TextureDesc desc{};
		desc.slot = resource.bindPoint;
		desc.stage = ShaderStageBind::VS;
		desc.texture = texture->GetGPUTextureHandle();

		gpuMat->boundTextures.push_back(desc);
	}

	if (allResolved)
		CPU->MarkClean();
}
