#pragma once
#include <EngineMinimal.h>
#include "RendererMinimal.h"

#include "Shader.h"

struct VSReflectData
{
	Vector<struct D3D11_INPUT_ELEMENT_DESC> inputDescs;
};

class Renderer;

class ShaderManager
{
public:
	ShaderManager(Renderer* InRenderer);
	~ShaderManager();

	GPUResourceHandle<Shader> CreateShader(const String& VertexPath, const String& FragmentPath);

	bool CompileShader(const String& VirtualPath, const String& ShaderModel, void** Blob);

	inline Shader* GetInternalShader(GPUResourceHandle<Shader> Handle)
	{
		return mShaderPool.Get(Handle);
	}

private:
	VSReflectData ReflectVSShader(ID3DBlob* Blob);

private:
	Renderer* mRenderer;
	ResourcePool<Shader> mShaderPool;
};