#pragma once
#include <EngineMinimal.h>
#include "RendererMinimal.h"

#include "Shader.h"

#define HLSL_VS_ENTRYPOINT "VSMain"
#define HLSL_FS_ENTRYPOINT "FSMain"
#define HLSL_VS_SUPPORTED_VERSION "vs_5_0"
#define HLSL_FS_SUPPORTED_VERSION "ps_5_0"

class Renderer;

class ShaderManager
{
public:
	ShaderManager(Renderer* InRenderer);
	~ShaderManager();

	GPUResourceHandle<ShaderProgram> TryGetShaderProgram(const Path& SourcePath);

	bool CompileShader(const Path& VirtualPath, const String& EntryPoint, const String& ShaderModel, ComPtr<ID3DBlob>& Blob);

	inline ShaderProgram* GetInternalShaderProgram(GPUResourceHandle<ShaderProgram> Handle)
	{
		return mShaderProgramPool.Get(Handle);
	}

private:
	ShaderReflectionInfo ReflectShader(ComPtr<ID3DBlob>& BytecodeBlob);

private:
	Renderer* mRenderer;
	ResourcePool<ShaderProgram> mShaderProgramPool;
	UnorderedMap<Path, GPUResourceHandle<ShaderProgram>> mPathToShaderProgram;
};