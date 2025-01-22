#pragma once
#include <UniqueID/UniqueID.h>

enum class TextureType : uint8
{
	eTexture2D,
	eTextureCube,

	eInvalid
};

class Texture
{
public:
	Texture();
	virtual ~Texture();

	void Destroy();
	const TextureType GetTextureType() const;

	virtual void Bind(uint32 aSlot) const = 0;
	virtual void Resize(const Vector2& /*aNewSize*/) {};
	/**
	* Used to pass the SRV to imgui.
	*/
	virtual void* GetUnderlyingSRV() = 0;

protected:
	uint32 mMipLevel;
	UniqueID mTextureID;
	Vector2 mSize;
	TextureType mType;
private:

};

class Texture2D : public Texture
{
public:
	Texture2D() { mType = TextureType::eTexture2D; };
	virtual ~Texture2D() {};

	virtual void* GetUnderlyingSRV() = 0;

private:

};

class TextureCube : public Texture
{
public:
	TextureCube() { mType = TextureType::eTextureCube; };
	virtual ~TextureCube() {};

private:

};