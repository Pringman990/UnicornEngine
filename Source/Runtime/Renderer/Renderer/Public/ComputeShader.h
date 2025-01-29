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
		const std::string& aShaderPath,
		const uint32& aStructuredDataSize, 
		const uint32& aStructuredDataCount, 
		const uint32& aTextureWidth,
		const uint32& aTextureHeight,
		void* aInitData
	);

	void UpdateStructuredBuffer(void* someStructuredData);
	void BindAndDispatch(const uint32 & aThreadGroupSizeX, const uint32 & aThreadGroupSizeY, const uint32 & aThreadGroupSizeZ = 1);

	ID3D11Texture2D* GetOutputTexture2D();
	Texture2D* GetTexture() { return mTexture; };
private:
	ComPtr<ID3D11Texture2D> mOutputTexture2D;
	ComPtr<ID3D11UnorderedAccessView> mUAV;
	ComPtr<ID3D11ShaderResourceView> mStructuredSRV;
	ComPtr<ID3D11Buffer> mStructuredBuffer;
	ComPtr<ID3D11ComputeShader> mShader;
	Texture2D* mTexture;
};