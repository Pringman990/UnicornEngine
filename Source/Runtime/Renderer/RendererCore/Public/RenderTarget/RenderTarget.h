#pragma once
#include "Texture/Texture.h"

class RenderTarget
{
	friend class RenderTargetResourceManager;
	friend class IRenderTargetFactory;
public:
	
	void Destroy();
	bool IsDepthTesting();
	Texture* GetTexture();

	virtual void Resize(Vector2 aSize) = 0;
	virtual void Bind() = 0;
	virtual void Clear() = 0;


protected:
	RenderTarget();
	virtual ~RenderTarget();

protected:
	Texture* mTexture;

	bool mIsDepthTesting;
};