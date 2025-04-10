#pragma once

class CommandList
{
public:
	CommandList();
	~CommandList();

	void BindTexture();
	void BindShader();
	void DrawMesh();

private:
	struct
	{
		ComPtr<ID3D12RootSignature> mRootSignature;
	} mGPURenderState;

	ComPtr<ID3D12GraphicsCommandList> mCommandList;
};