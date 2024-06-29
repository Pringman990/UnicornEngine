#pragma once

class SpriteShader;
class Texture;

class SpriteInstance
{
public:
	SpriteInstance();
	~SpriteInstance();

	void Init(const std::wstring& aTexturePath = L"");
	void Draw();
	void SetTexture(const std::wstring& aTexturePath);

	std::shared_ptr<Texture> GetTexture();
	Transform& GetTransform();
	void SetTransform(const Transform& aTransform);

private:
	std::shared_ptr<SpriteShader> mSpriteShader;
	std::shared_ptr<Texture> mTexture;
	std::wstring mTexturePath;
	Transform mTransform;
};