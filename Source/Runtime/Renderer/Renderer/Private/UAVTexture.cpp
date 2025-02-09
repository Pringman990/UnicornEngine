#include "pch.h"
#include "UAVTexture.h"

#include "Texture2D.h"

UAVTexture::UAVTexture()
{
}

UAVTexture::~UAVTexture()
{
}

std::shared_ptr<UAVTexture> UAVTexture::Create(const Vector2& aSize)
{
	std::shared_ptr<UAVTexture> UavTexture = std::make_shared<UAVTexture>();

	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = static_cast<uint32>(aSize.x); 
	texDesc.Height = static_cast<uint32>(aSize.y);
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	ID3D11Texture2D* texture2d;
	Renderer::GetInstance()->GetDevice()->CreateTexture2D(&texDesc, nullptr, &texture2d);

	UavTexture->mTexture2D = Texture2D::Create(texture2d);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = texDesc.Format;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;

	Renderer::GetInstance()->GetDevice()->CreateUnorderedAccessView(texture2d, &uavDesc, UavTexture->mUAV.GetAddressOf());

	return UavTexture;
}

void UAVTexture::BindPS(uint32 /*aSlot*/) const
{
	_LOG_RENDERER_WARNING("UAVTextures cant be bound to pixel shaders");
}

void UAVTexture::BindCS(uint32 aSlot) const
{
	Renderer::GetInstance()->GetDeviceContext()->CSSetUnorderedAccessViews(aSlot, 1, mUAV.GetAddressOf(), nullptr);
}

void UAVTexture::UnbindCS(uint32 aSlot) const
{
	ID3D11UnorderedAccessView* null = nullptr;
	Renderer::GetInstance()->GetDeviceContext()->CSSetUnorderedAccessViews(aSlot, 1, &null, nullptr);
}

Texture2D* UAVTexture::GetTexture2D()
{
	return mTexture2D;
}
