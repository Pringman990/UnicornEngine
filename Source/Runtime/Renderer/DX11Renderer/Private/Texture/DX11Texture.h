#pragma once
#include "Texture/Texture.h"

class DX11Texture2D : public Texture2D
{
	friend class DX11TextureFactory;
public:
	DX11Texture2D();
	~DX11Texture2D() override;

	void Bind(uint32 aSlot) const override;
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

class DX11TextureCube : public TextureCube
{
	friend class DX11TextureFactory;
public:
	DX11TextureCube();
	~DX11TextureCube() override;

	virtual void Bind(uint32 aSlot) const override;

	/**
	* Used to pass the SRV to imgui.
	*/
	virtual void* GetUnderlyingSRV() override;
private:
	ComPtr<ID3D11ShaderResourceView> mSRV;
	ComPtr<ID3D11Texture2D> mTextureCube;
};