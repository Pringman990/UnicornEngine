#pragma once

#include <IResourceManager.h>
#include "Shader/IShaderFactory.h"
#include "Shader/Shader.h"

class MaterialShader;

class ShaderResourceManager : public IResourceManager
{
public:
	ShaderResourceManager();
	~ShaderResourceManager() override;

	void Init();

	Shader* TryGetShader(
		eShaderTypes aShaderType, 
		const std::string& aFileName,
		const std::string& aEntryPoint = "main",
		const std::string& aModel = "vs_5_0"
	);

	MaterialShader* GetDefualtMaterialShader();

	void SetShaderFactory(IShaderFactory* aFactory);
private:
	
	MaterialShader* TryGetMaterialShader(
		const std::string& aFileName,
		const std::string& aEntryPoint = "main",
		const std::string& aModel = "vs_5_0"
	);

	Shader* GetVertexShader(
		const std::string& aFileName,
		const std::string& aEntryPoint = "main",
		const std::string& aModel = "vs_5_0"
	);

	Shader* GetPixelShader(
		const std::string& aFileName,
		const std::string& aEntryPoint = "main",
		const std::string& aModel = "ps_5_0"
	);

private:
	std::unordered_map<ShaderKey, Shader*> mCachedShaders;
	MaterialShader* mDefualtMaterialShader;
	IShaderFactory* mShaderFactory;
};