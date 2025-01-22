#pragma once
#include <Texture/ITextureFactory.h>

class DX11Texture2D;

class DX11TextureFactory : public ITextureFactory
{
public:
	DX11TextureFactory();
	~DX11TextureFactory() override;

	virtual Texture* LoadTextureFromFile(const std::string& aPath) override;
	static DX11Texture2D* CreateTexture2D(const Vector2& aSize);

	/// <summary>
	///  Used for resizing a DX11Texture2D.
	///  So the argument must be a valid non-nullptr pointer.
	/// </summary>
	static bool CreateTexture2D(DX11Texture2D* aTexture, D3D11_TEXTURE2D_DESC aTextureDesc, D3D11_SHADER_RESOURCE_VIEW_DESC aSRVDesc);

	/// <summary>
	/// Used when creating the backbuffer
	/// </summary>
	static DX11Texture2D* CreateTexture2D(ID3D11Texture2D* aTexture);

private:

};