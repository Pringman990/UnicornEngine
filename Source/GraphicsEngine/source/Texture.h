#pragma once
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

struct ID3D11ShaderResourceView;

class Texture
{
public:
	Texture();
	~Texture();

	bool Create(const wchar_t* aPath);
	void Bind(int aSlot);

	bool HasCreated() { return mHasBeenCreated; };

	Vector2 GetTextureSize() const;
	ID3D11ShaderResourceView* GetSRV() const;

private:
	ComPtr<ID3D11ShaderResourceView> mSRV;
	bool mHasBeenCreated;

	std::wstring mDefaultMaterialTexturePath;
};