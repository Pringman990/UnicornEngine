#include "pch.h"
#include "DX11VertexShaderBackend.h"

DX11VertexShaderBackend::DX11VertexShaderBackend(
	ID3D11VertexShader* aPtr,
	ComPtr<ID3DBlob>& aBlob
)
	:
	Shader(eShaderTypes::eVertex),
	mDX11VertexShader(aPtr),
	mDX11Blob(aBlob)
{
}

DX11VertexShaderBackend::~DX11VertexShaderBackend()
{
	mDX11VertexShader->Release();
	mDX11Blob->Release();
}

bool DX11VertexShaderBackend::Compile()
{
	return false;
}

bool DX11VertexShaderBackend::Bind()
{
	ID3D11DeviceContext* context = DX11RenderingBackend::_GetInstance()->GetDeviceContext();
	context->VSSetShader(mDX11VertexShader.Get(), nullptr, 0);
	return true;
}

ComPtr<ID3DBlob> DX11VertexShaderBackend::GetVertexBlob()
{
	return mDX11Blob;
}
