#include "pch.h"
#include "DX11RenderTargetBackend.h"

#include "DX11RenderTargetFactory.h"

DX11RenderTargetBackend::DX11RenderTargetBackend()
	:
	mSRV(nullptr),
	mRTV(nullptr),
	mDSV(nullptr),
	mDSS(nullptr),
	mTexture2D(nullptr),
	mViewport(nullptr)
{
	mViewport = new D3D11_VIEWPORT();
}

DX11RenderTargetBackend::~DX11RenderTargetBackend()
{

}

void DX11RenderTargetBackend::Resize(Vector2 aSize)
{
	Release();
	DX11RenderTargetFactory::_CreateRenderTarget(aSize, this);
}

void DX11RenderTargetBackend::Release()
{
	if (mRTV)
		mRTV->Release();
	if (mSRV)
		mSRV->Release();
	if (mDSV)
		mDSV->Release();
	if (mDSS)
		mDSS->Release();
	if (mTexture2D)
		mTexture2D->Release();
}

ID3D11ShaderResourceView* DX11RenderTargetBackend::GetSRV()
{
	return mSRV.Get();
}

ID3D11ShaderResourceView** DX11RenderTargetBackend::GetAdressOfSRV()
{
	return mSRV.GetAddressOf();
}

ID3D11RenderTargetView* DX11RenderTargetBackend::GetRTV()
{
	return mRTV.Get();
}

ID3D11RenderTargetView** DX11RenderTargetBackend::GetAdressOfRTV()
{
	return mRTV.GetAddressOf();
}

ID3D11DepthStencilView* DX11RenderTargetBackend::GetDSV()
{
	return mDSV.Get();
}

ID3D11DepthStencilView** DX11RenderTargetBackend::GetAdressOfDSV()
{
	return mDSV.GetAddressOf();
}

ID3D11DepthStencilState* DX11RenderTargetBackend::GetDSS()
{
	return mDSS.Get();
}

ID3D11DepthStencilState** DX11RenderTargetBackend::GetAdressOfDSS()
{
	return mDSS.GetAddressOf();
}

ID3D11Texture2D* DX11RenderTargetBackend::GetTexture2D()
{
	return mTexture2D.Get();
}

ID3D11Texture2D** DX11RenderTargetBackend::GetAddressOfTexture2D()
{
	return mTexture2D.GetAddressOf();
}

D3D11_VIEWPORT* DX11RenderTargetBackend::GetViewPort()
{
	return mViewport;
}
