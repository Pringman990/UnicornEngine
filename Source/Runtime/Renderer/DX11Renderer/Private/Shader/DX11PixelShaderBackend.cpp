#include "pch.h"
#include "DX11PixelShaderBackend.h"

DX11PixelShaderBackend::DX11PixelShaderBackend(ID3D11PixelShader* aPtr)
    :
    Shader(eShaderTypes::ePixel),
    mDX11PixelShader(aPtr)
{
}

DX11PixelShaderBackend::~DX11PixelShaderBackend()
{
    mDX11PixelShader->Release();
}

bool DX11PixelShaderBackend::Compile()
{
    return false;
}

bool DX11PixelShaderBackend::Bind()
{
    ID3D11DeviceContext* context = DX11RenderingBackend::_GetInstance()->GetDeviceContext();
    context->PSSetShader(mDX11PixelShader.Get(), nullptr, 0);

    return true;
}
