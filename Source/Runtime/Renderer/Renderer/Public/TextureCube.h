#pragma once
#include "Texture.h"

class TextureCube : public Texture
{
public:
	TextureCube();
	~TextureCube();

	void BindPS(uint32 aSlot) const override;
	void BindCS(uint32 aSlot) const override;
	void UnbindCS(uint32 aSlot) const override;
	ID3D11ShaderResourceView* GetSRV();

	static TextureCube* Create(const std::string& aPath);

private:
	ComPtr<ID3D11ShaderResourceView> mSRV;
	ComPtr<ID3D11Texture2D> mTextureCube;
	Vector2 mSize;
	uint32 mMipLevel;
};