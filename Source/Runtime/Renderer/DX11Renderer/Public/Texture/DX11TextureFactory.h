#pragma once
#include <Texture/ITextureFactory.h>

class DX11Texture;

class DX11TextureFactory : public ITextureFactory
{
public:
	DX11TextureFactory();
	~DX11TextureFactory() override;

	virtual Texture* CreateTexture(const std::string& aPath) override;
	static DX11Texture* CreateTexture(const Vector2& aSize);

	/// <summary>
	///  Used for resizing a DX11Texture.
	///  So the argument must be a valid non-nullptr pointer.
	/// </summary>
	static bool CreateTexture(DX11Texture* aTexture, D3D11_TEXTURE2D_DESC aTextureDesc, D3D11_SHADER_RESOURCE_VIEW_DESC aSRVDesc);

	/// <summary>
	/// Used when creating the backbuffer
	/// </summary>
	static DX11Texture* CreateTexture(ID3D11Texture2D* aTexture);

private:

};