#pragma once
#include "Texture.h"

class Texture2D : public Texture
{
public:
	Texture2D();
	~Texture2D();

	void BindPS(uint32 aSlot) const override;
	void BindCS(uint32 aSlot) const override;
	void UnbindCS(uint32 aSlot) const override;
	void Resize(const Vector2& aNewSize);

	void Release();

	static Texture2D* Create(const std::wstring& aDDSPath);
	static Texture2D* Create(const Vector2& aSize);

	/// <summary>
	///  Used for resizing a DX11Texture2D.
	///  So the argument must be a valid non-nullptr pointer.
	/// </summary>
	static bool Create(Texture2D* aTexture, D3D11_TEXTURE2D_DESC aTextureDesc, D3D11_SHADER_RESOURCE_VIEW_DESC aSRVDesc);
	
	static Texture2D* Create(D3D11_TEXTURE2D_DESC aTextureDesc);

	/// <summary>
	/// Used when creating the backbuffer
	/// </summary>
	static Texture2D* Create(ID3D11Texture2D* aTexture);

	ID3D11ShaderResourceView* GetSRV();
	ID3D11ShaderResourceView** GetAddressOfSRV();
	ID3D11Texture2D* GetTexture2D();
	ID3D11Texture2D** GetAdressOfTexture2D();

	D3D11_TEXTURE2D_DESC GetTexture2DDesc() const;

private:
	ComPtr<ID3D11ShaderResourceView> mSRV;
	ComPtr<ID3D11Texture2D> mTexture2D;

	Vector2 mSize;
	uint32 mMipLevel;
};