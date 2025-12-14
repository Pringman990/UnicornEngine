#pragma once
#include <EngineMinimal.h>
#include "RendererMinimal.h"

struct ID3D11Texture2D;
struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;

struct GPUTexture
{
	ComPtr<ID3D11Texture2D> texture;
	ComPtr<ID3D11RenderTargetView> rtv;
	ComPtr<ID3D11ShaderResourceView> srv;
	ComPtr<ID3D11DepthStencilView> dsv;
	Vector3i extent;
	RenderFormat format;
	TextureUsage usage;
	TextureBindFlags binds;
	D3D11_VIEWPORT viewport;

	static void Reset(GPUTexture* Texture)
	{
		if (Texture->texture)
			Texture->texture.Reset();

		if (Texture->rtv)
			Texture->rtv.Reset();

		if (Texture->srv)
			Texture->srv.Reset();

		if (Texture->dsv)
			Texture->dsv.Reset();

		Texture->extent = Vector3i();
		Texture->format = RenderFormat::UNKNOWN;
		Texture->usage = TextureUsage::Undefined;
		Texture->binds = TextureBindFlags::Undefined;
		Texture->viewport = {};
	}
};