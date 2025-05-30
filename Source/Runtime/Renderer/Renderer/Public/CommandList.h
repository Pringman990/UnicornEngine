#pragma once
#include <Core.h>

class CommandList
{
public:
	CommandList();
	~CommandList();

	void SetTexture();
	void SetShader();
	void DrawMesh();

private:
	struct
	{
		ComPtr<ID3D12RootSignature> mRootSignature;
	} mGPURenderState;

	ComPtr<ID3D12GraphicsCommandList> mCommandList;
};