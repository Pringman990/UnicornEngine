#pragma once
#include <Shader/Shader.h>

class DX11VertexShaderBackend : public Shader
{
public:
	DX11VertexShaderBackend(
		ID3D11VertexShader* aPtr,
		ComPtr<ID3DBlob>& aBlob
	);

	~DX11VertexShaderBackend() override;

	virtual bool Compile() override;
	virtual bool Bind() override;

	ComPtr<ID3DBlob> GetVertexBlob();

private:
	ComPtr<ID3D11VertexShader> mDX11VertexShader;
	ComPtr<ID3DBlob> mDX11Blob;
};