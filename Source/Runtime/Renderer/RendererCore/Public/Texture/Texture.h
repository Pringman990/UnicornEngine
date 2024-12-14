#pragma once
#include <UniqueID.h>

enum class eTextureType
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
	eTextureType GetType();
	virtual void Resize(const Vector2& aNewSize) = 0;

	/**
	* Used to pass the SRV to imgui.
	*/
	virtual void* GetUnderlyingSRV() = 0;

protected:
	eTextureType mType;
	uint32 mMipLevel;
	UniqueID mTextureID;
	Vector2 mSize;
private:

};