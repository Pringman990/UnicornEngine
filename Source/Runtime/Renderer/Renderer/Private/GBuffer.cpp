#include "pch.h"
#include "GBuffer.h"

GBuffer::GBuffer()
{
}

GBuffer::~GBuffer()
{
	for (int32 i = 0; i < RenderTargetType::eCount; i++)
	{
		if (mTextures[i])
		{
			delete mTextures[i];
			mTextures[i] = nullptr;
		}
	}
}

std::shared_ptr<GBuffer> GBuffer::Create(const Vector2& aSize)
{
	ID3D11Device* device = Renderer::GetInstance()->GetDevice();

	std::array<DXGI_FORMAT, RenderTargetType::eCount> textureFormats =
	{
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,// Albedo
		DXGI_FORMAT_R10G10B10A2_UNORM,// Normal
		DXGI_FORMAT_R32G32B32A32_FLOAT,// Position
	};

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = static_cast<uint32>(aSize.x);
	textureDesc.Height = static_cast<uint32>(aSize.y);
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;


	std::shared_ptr<GBuffer> gbuffer = std::make_shared<GBuffer>();
	for (int32 i = 0; i < RenderTargetType::eCount; i++)
	{
		textureDesc.Format = textureFormats[i];

		Texture2D* texture = Texture2D::Create(textureDesc);
		if (!texture)
		{
			_LOG_RENDERER_ERROR("Failed to create Texture2D");
			return nullptr;
		}
		gbuffer->mTextures[i] =texture;

		HRESULT hr = device->CreateRenderTargetView(texture->GetTexture2D(), nullptr, gbuffer->mRenderTargetViews[i].GetAddressOf());
		if (FAILED(hr))
		{
			_LOG_RENDERER_ERROR("Failed to create RenderTargetView: {}", hr);
			return nullptr;
		}
	}

	D3D11_TEXTURE2D_DESC depthDesc = {};
	depthDesc.Width = textureDesc.Width;
	depthDesc.Height = textureDesc.Height;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	
	ID3D11Texture2D* depthTexture;
	HRESULT hr = device->CreateTexture2D(&depthDesc, nullptr, &depthTexture);
	if (FAILED(hr))
	{
		_LOG_RENDERER_ERROR("Failed to create DepthStencil Texture2D: {}", hr);
		return nullptr;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = depthDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	hr = device->CreateDepthStencilView(depthTexture, &dsvDesc, gbuffer->mDSV.GetAddressOf());
	if (FAILED(hr))
	{
		_LOG_RENDERER_ERROR("Failed to create DepthStencilView: {}", hr);
		return nullptr;
	}

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	hr = device->CreateDepthStencilState(&depthStencilDesc, gbuffer->mDSS.GetAddressOf());
	if (FAILED(hr))
	{
		_LOG_RENDERER_ERROR("Failed to create mDSSWriteZero: {}", hr);
		return nullptr;
	}

	gbuffer->mViewport = { 0 };
	gbuffer->mViewport.TopLeftX = 0.0f;
	gbuffer->mViewport.TopLeftY = 0.0f;
	gbuffer->mViewport.Width = aSize.x;
	gbuffer->mViewport.Height = aSize.y;
	gbuffer->mViewport.MinDepth = 0.0f;
	gbuffer->mViewport.MaxDepth = 1.0f;

	return gbuffer;
}

void GBuffer::BindRenderTargets()
{
	ID3D11DeviceContext* context = Renderer::GetInstance()->GetDeviceContext();
	context->OMSetRenderTargets(RenderTargetType::eCount, mRenderTargetViews[0].GetAddressOf(), mDSV.Get());
	context->RSSetViewports(1, &mViewport);
}

void GBuffer::UnbindRenderTargets()
{
	ID3D11DeviceContext* context = Renderer::GetInstance()->GetDeviceContext();
	ID3D11RenderTargetView* nullSRVs[RenderTargetType::eCount] = {nullptr};
	context->OMSetRenderTargets(RenderTargetType::eCount, nullSRVs, nullptr);
}

void GBuffer::ClearRenderTargets()
{
	ID3D11DeviceContext* context = Renderer::GetInstance()->GetDeviceContext();
	for (int32 i = 0; i < RenderTargetType::eCount; i++)
	{
		context->ClearRenderTargetView(mRenderTargetViews[i].Get(), Color(0,0,0,0));
	}
	context->ClearDepthStencilView(mDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void GBuffer::BindTexturesToPS(uint32 aStartSlot)
{
	for (int32 i = 0; i < RenderTargetType::eCount; i++)
	{
		mTextures[i]->BindPS(aStartSlot + i);
	}
}

void GBuffer::BindTexturesToCS(uint32 aStartSlot)
{
	for (int32 i = 0; i < RenderTargetType::eCount; i++)
	{
		mTextures[i]->BindCS(aStartSlot + i);
	}
}

void GBuffer::UnbindTexturesFromCS(uint32 aStartSlot)
{
	for (int32 i = 0; i < RenderTargetType::eCount; i++)
	{
		mTextures[i]->UnbindCS(aStartSlot + i);
	}
}

Texture2D* GBuffer::GetAlbedoTexture()
{
	return mTextures[eAlbedo];
}

Texture2D* GBuffer::GetNormalTexture()
{
	return mTextures[eNormal];
}

Texture2D* GBuffer::GetWorldPositionTexture()
{
	return mTextures[eWorldPosition];
}
