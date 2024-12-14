#pragma once
#include <Shader/InputLayout.h>

class DX11InputLayout : public InputLayout
{
	friend class DX11InputLayoutFactory;

public:
	DX11InputLayout(eInputLayoutStandardTypes aType);
	~DX11InputLayout() override;

	virtual void Bind() override;

private:
	ComPtr<ID3D11InputLayout> mLayout;
};