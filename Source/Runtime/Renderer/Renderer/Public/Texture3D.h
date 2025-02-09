#pragma once
#include "Texture.h"
#include <d3d11.h>
#include <Utility/Array3D.h>
#include "Renderer.h"

class Texture3D : public Texture
{
public:
	Texture3D();
	~Texture3D();

	void BindPS(uint32 aSlot) const override;
	void BindCS(uint32 aSlot) const override;
	void UnbindCS(uint32 aSlot) const override;
	void Resize(const Vector3& aNewSize);

	void Release();

	static Texture3D* Create(Array3D<uint8>& aArray3D);
	static Texture3D* CreateUAV(const Vector3& aSize);

	/// <summary>
	///  Used for resizing a DX11Texture2D.
	///  So the argument must be a valid non-nullptr pointer.
	/// </summary>
	static bool Create(Texture3D* aTexture, D3D11_TEXTURE3D_DESC aTextureDesc, D3D11_SHADER_RESOURCE_VIEW_DESC aSRVDesc);


	ID3D11ShaderResourceView* GetSRV();
	ID3D11ShaderResourceView** GetAddressOfSRV();
	ID3D11Texture3D* GetTexture3D();
	ID3D11Texture3D** GetAdressOfTexture3D();
	D3D11_TEXTURE3D_DESC GetTexture3DDesc() const;

private:
	ComPtr<ID3D11ShaderResourceView> mSRV;
	ComPtr<ID3D11Texture3D> mTexture3D;

	Vector3 mSize;
	uint32 mMipLevel;
};