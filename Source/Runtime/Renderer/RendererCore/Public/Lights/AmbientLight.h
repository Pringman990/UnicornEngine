#pragma once

class TextureCube;

class AmbientLight
{
public:
	AmbientLight();
	~AmbientLight();

	void SetCubemap(TextureCube* aCubemap);
	const TextureCube* GetCubemap() const;

	const Color GetSolidColor() const;

private:
	TextureCube* mCubemap;
	Color mSolidColor;
};