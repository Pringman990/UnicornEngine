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
    Vector2i extent;
    RenderFormat format;
    TextureUsage usage;
    D3D11_VIEWPORT viewport;
};