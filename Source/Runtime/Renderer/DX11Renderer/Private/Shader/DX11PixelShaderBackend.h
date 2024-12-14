#pragma once
#include <Shader/Shader.h>

class DX11PixelShaderBackend : public Shader
{
public:
	DX11PixelShaderBackend(ID3D11PixelShader* aPtr);
	~DX11PixelShaderBackend() override;

	virtual bool Compile() override;
	virtual bool Bind() override;

private:
	ComPtr<ID3D11PixelShader> mDX11PixelShader;
};