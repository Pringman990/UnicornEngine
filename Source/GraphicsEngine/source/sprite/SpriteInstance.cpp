#include "GraphicsPch.h"
#include "SpriteInstance.h"

#include "../shaders/SpriteShader.h"
#include "../Texture.h"

SpriteInstance::SpriteInstance()
{

}

SpriteInstance::~SpriteInstance()
{
}

void SpriteInstance::Init(const std::wstring& aTexturePath)
{
	mSpriteShader = std::make_shared<SpriteShader>();
	if (!mSpriteShader->CreateShader(L"sprite_VS.hlsl", L"sprite_PS.hlsl"))
		return;

	mTexture = std::make_shared<Texture>();
	SetTexture(aTexturePath);
}

void SpriteInstance::Draw()
{
	if (!mTexture || !mSpriteShader)
		return;

	mTexture->Bind(1);

	mSpriteShader->Render(mTransform);
#ifdef _DEBUG
	Engine::GetGraphicsEngine().AddDrawCall();
#endif // _DEBUG
}

void SpriteInstance::SetTexture(const std::wstring& aTexturePath)
{
	if (!mTexture->Create(aTexturePath.c_str()))
	{
		std::cout << "Failed to set Texture" << std::endl;
	}

	mTexturePath = aTexturePath;
}

Transform& SpriteInstance::GetTransform()
{
	return mTransform;
}

void SpriteInstance::SetTransform(const Transform& aTransform)
{
	mTransform = aTransform;
}

std::shared_ptr<Texture> SpriteInstance::GetTexture()
{
	return mTexture;
}
