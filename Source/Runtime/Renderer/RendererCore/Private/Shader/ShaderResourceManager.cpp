#include "pch.h"
#include "Shader/ShaderResourceManager.h"

#include "Shader/MaterialShader.h"

ShaderResourceManager::ShaderResourceManager()
	:
	mShaderFactory(nullptr)
{
}

ShaderResourceManager::~ShaderResourceManager()
{
	for (auto& it : mCachedShaders)
	{
		delete it.second;
		it.second = nullptr;
	}
	mCachedShaders.clear();

	delete mDefualtMaterialShader;
	mDefualtMaterialShader = nullptr;

	delete mShaderFactory;
	mShaderFactory = nullptr;
}

void ShaderResourceManager::Init()
{
	MaterialShader* materialShader = new MaterialShader();

	materialShader->SetVertexShader(GetVertexShader("../../Source/Runtime/Renderer/RawShaders/Public/Lit_Model_VS.hlsl"));
	materialShader->SetPixelShader(GetPixelShader("../../Source/Runtime/Renderer/RawShaders/Public/Lit_Model_PS.hlsl"));

	mDefualtMaterialShader = materialShader;
}

Shader* ShaderResourceManager::TryGetShader(eShaderTypes aShaderType, const std::string& aFileName, const std::string& aEntryPoint, const std::string& aModel)
{
	switch (aShaderType)
	{
	case eShaderTypes::eMatrial:
	{
		if (aFileName == "__UNCE_Defualt_Material")
			return mDefualtMaterialShader;

		Shader* materialShader = TryGetMaterialShader(aFileName, aEntryPoint, aModel);
		return materialShader;
	}
	case eShaderTypes::ePixel:
	{
		return GetPixelShader(aFileName, aEntryPoint);
	}
	case eShaderTypes::eVertex:
	{
		return GetVertexShader(aFileName, aEntryPoint);
	}
	case eShaderTypes::eInvalid:
	{
		break;
	}
	default:
		return nullptr;
	}

	return nullptr;
}

MaterialShader* ShaderResourceManager::GetDefualtMaterialShader()
{
	return mDefualtMaterialShader;
}

MaterialShader* ShaderResourceManager::TryGetMaterialShader(const std::string& aFileName, const std::string& aEntryPoint, const std::string& aModel)
{
	ShaderKey key(aFileName, aEntryPoint, aModel, eShaderTypes::eMatrial);

	auto it = mCachedShaders.find(key);
	if (it != mCachedShaders.end())
	{
		return static_cast<MaterialShader*>(it->second);
	}

	Shader* vertexShader = GetVertexShader(aFileName, aEntryPoint);
	Shader* pixelShader = GetPixelShader(aFileName, aEntryPoint);

	_ENSURE_RENDERER(vertexShader, "Vertex Shader was null when retriving");
	_ENSURE_RENDERER(pixelShader, "Vertex Shader was null when retriving");

	MaterialShader* materialShader = new MaterialShader();
	materialShader->SetVertexShader(vertexShader);
	materialShader->SetPixelShader(pixelShader);

	mCachedShaders[key] = materialShader;

	return materialShader;
}

Shader* ShaderResourceManager::GetVertexShader(const std::string& aFileName, const std::string& aEntryPoint, const std::string& aModel)
{
	ShaderKey key(aFileName, aEntryPoint, aModel, eShaderTypes::eVertex);
	auto it = mCachedShaders.find(key);
	if (it != mCachedShaders.end())
	{
		return it->second;
	}

	Shader* shader = mShaderFactory->CreateVertexShader(aFileName, aEntryPoint, aModel);
	mCachedShaders[key] = shader;
	return shader;
}

Shader* ShaderResourceManager::GetPixelShader(const std::string& aFileName, const std::string& aEntryPoint, const std::string& aModel)
{
	ShaderKey key(aFileName, aEntryPoint, aModel, eShaderTypes::ePixel);
	auto it = mCachedShaders.find(key);
	if (it != mCachedShaders.end())
	{
		return it->second;
	}

	Shader* shader = mShaderFactory->CreatePixelShader(aFileName, aEntryPoint, aModel);
	mCachedShaders[key] = shader;
	return shader;
}

void ShaderResourceManager::SetShaderFactory(IShaderFactory* aFactory)
{
	mShaderFactory = aFactory;
}