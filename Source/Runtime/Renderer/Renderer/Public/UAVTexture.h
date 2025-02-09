#pragma once
#include "Texture.h"

class Texture2D;

class UAVTexture : public Texture
{
public:
	UAVTexture();
	~UAVTexture() override;

	static std::shared_ptr<UAVTexture> Create(const Vector2& aSize);

	virtual void BindPS(uint32 aSlot) const override;
	virtual void BindCS(uint32 aSlot) const override;
	virtual void UnbindCS(uint32 aSlot) const override;

	Texture2D* GetTexture2D();

private:
	ComPtr<ID3D11UnorderedAccessView> mUAV;
	Texture2D* mTexture2D;
};
