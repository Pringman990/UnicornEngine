#pragma once
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

struct ID3D11ShaderResourceView;
struct ID3D11RenderTargetView;
struct ID3D11Texture2D;

class RenderTarget
{
public:
	RenderTarget();
	~RenderTarget();

	bool Create(Vector2 aSize);

private:
	ComPtr<ID3D11ShaderResourceView> mSRV;
	ComPtr<ID3D11RenderTargetView> mRTV;
	ComPtr<ID3D11Texture2D> mTexture2D;
	Vector2 mSize;
};