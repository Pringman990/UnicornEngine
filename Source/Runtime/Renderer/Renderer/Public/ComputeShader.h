#pragma once
#include <d3d11.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

class Texture2D;

class ComputeShader
{
public:
	ComputeShader();
	~ComputeShader();

	bool Init(
		const std::string& aShaderPath
	);

	void Dispatch(const uint32 & aThreadGroupSizeX, const uint32 & aThreadGroupSizeY, const uint32 & aThreadGroupSizeZ = 1);

private:
	ComPtr<ID3D11ComputeShader> mShader;
};