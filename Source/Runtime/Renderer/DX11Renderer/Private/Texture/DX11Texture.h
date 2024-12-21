#pragma once
#include "Texture/Texture.h"

class DX11Texture : public Texture
{
	friend class DX11TextureFactory;
public:
	DX11Texture();
	~DX11Texture() override;

	void Bind(uint32 aSlot) override;
	virtual void Resize(const Vector2& aNewSize) override;

	void Release();

	virtual void* GetUnderlyingSRV() override;

	ID3D11ShaderResourceView* GetSRV();
	ID3D11ShaderResourceView** GetAddressOfSRV();
	ID3D11Texture2D* GetTexture2D();
	ID3D11Texture2D** GetAdressOfTexture2D();

	D3D11_TEXTURE2D_DESC GetTexture2DDesc() const;

private:
	ComPtr<ID3D11ShaderResourceView> mSRV;
	ComPtr<ID3D11Texture2D> mTexture2D;
};