#pragma once
#include <EngineMinimal.h>
#include "RendererMinimal.h"

#include "InputLayout.h"

struct Shader
{
    ComPtr<struct ID3D11VertexShader> vs;
    ComPtr<struct ID3D11PixelShader> ps;
    InputLayout* layout;
};