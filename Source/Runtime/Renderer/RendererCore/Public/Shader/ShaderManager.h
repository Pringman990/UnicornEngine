#pragma once

#include "Shader/IShaderFactory.h"
#include "Shader/Shader.h"

class MaterialShader;

class ShaderManager
{
public:
	void Init();

	Shader* TryGetShader(
		eShaderTypes aShaderType, 
		const std::string& aFileName,
		const std::string& aEntryPoint = "main",
		const std::string& aModel = "vs_5_0"
	);

	MaterialShader* TryGetMaterialShader(
		const std::string& aPixelShaderFileName,
		const std::string& aVertexShaderFileName
	);

	MaterialShader* GetDefualtMaterialShader();

	void SetShaderFactory(IShaderFactory* aFactory);
private:
	friend class Renderer;
	friend class RendererFactory;
	ShaderManager();
	~ShaderManager();
private:
	
	MaterialShader* TryGetMaterialShader(
		const std::string& aFileName,
		const std::string& aEntryPoint,
		const std::string& aModel
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