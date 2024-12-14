#include "pch.h"
#include "DX11RenderTarget.h"

#include "RenderTarget/DX11RenderTargetFactory.h"
#include "Texture/DX11Texture.h"

DX11RenderTarget::DX11RenderTarget()
	:
	mRTV(nullptr),
	mDSV(nullptr),
	mDSS(nullptr),
	mViewport({ 0 })
{
}

DX11RenderTarget::~DX11RenderTarget()
{
	delete mTexture;
	mTexture = nullptr;

	mRTV.Reset();
	mDSV.Reset();
	mDSS.Reset();
	mDepthTexture.Reset();
}

void DX11RenderTarget::Resize(Vector2 aSize)
{
	D3D11_RENDER_TARGET_VIEW_DESC desc;
	mRTV->GetDesc(&desc);

	D3D11_TEXTURE2D_DESC depthTexture = {0};
	D3D11_DEPTH_STENCIL_VIEW_DESC depthDesc = { };
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = { 0 };
	if (mIsDepthTesting)
	{
		mDepthTexture->GetDesc(&depthTexture);
		mDSV->GetDesc(&depthDesc);
		mDSS->GetDesc(&depthStencilDesc);
	}

	mTexture->Resize(aSize);

	Release();

	DX11RenderTargetFactory::_CreateRenderTarget(aSize, this, desc, depthTexture, depthDesc, depthStencilDesc);
}

void DX11RenderTarget::Release()
{
	if (mRTV)
		mRTV->Release();
	if (mDSV)
		mDSV->Release();
	if (mDSS)
		mDSS->Release();
	if (mDepthTexture)
		mDepthTexture->Release();
}

void DX11RenderTarget::Bind()
{
	DX11RenderingBackend::_GetInstance()->GetDeviceContext()->OMSetDepthStencilState(mDSS.Get(), 0);
	DX11RenderingBackend::_GetInstance()->GetDeviceContext()->OMSetRenderTargets(1, mRTV.GetAddressOf(), mDSV.Get());
	DX11RenderingBackend::_GetInstance()->GetDeviceContext()->RSSetViewports(1, &mViewport);
}

void DX11RenderTarget::Clear()
{
	Vector4 color(0, 0, 0, 1);
	DX11RenderingBackend::_GetInstance()->GetDeviceContext()->ClearRenderTargetView(mRTV.Get(), (float*)&color);
	DX11RenderingBackend::_GetInstance()->GetDeviceContext()->ClearDepthStencilView(mDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

ID3D11RenderTargetView* DX11RenderTarget::GetRTV()
{
	return mRTV.Get();
}

ID3D11RenderTargetView** DX11RenderTarget::GetAdressOfRTV()
{
	return mRTV.GetAddressOf();
}

ID3D11DepthStencilView* DX11RenderTarget::GetDSV()
{
	return mDSV.Get();
}

ID3D11DepthStencilView** DX11RenderTarget::GetAdressOfDSV()
{
	return mDSV.GetAddressOf();
}

ID3D11DepthStencilState* DX11RenderTarget::GetDSS()
{
	return mDSS.Get();
}

ID3D11DepthStencilState** DX11RenderTarget::GetAdressOfDSS()
{
	return mDSS.GetAddressOf();
}

ID3D11Texture2D* DX11RenderTarget::GetDepthTexture()
{
	return mDepthTexture.Get();
}

ID3D11Texture2D** DX11RenderTarget::GetAdressOfDepthTexture()
{
	return mDepthTexture.GetAddressOf();
}

D3D11_VIEWPORT* DX11RenderTarget::GetViewPort()
{
	return &mViewport;
}
