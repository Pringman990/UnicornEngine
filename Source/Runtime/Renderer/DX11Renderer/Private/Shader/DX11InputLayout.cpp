#include "pch.h"
#include "DX11InputLayout.h"

DX11InputLayout::DX11InputLayout(eInputLayoutStandardTypes aType) : InputLayout(aType)
{
}

DX11InputLayout::~DX11InputLayout()
{
}

void DX11InputLayout::Bind()
{
	ID3D11DeviceContext* context = DX11RenderingBackend::_GetInstance()->GetDeviceContext();
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(mLayout.Get());
}
