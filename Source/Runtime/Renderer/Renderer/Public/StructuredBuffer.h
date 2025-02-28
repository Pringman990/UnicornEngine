#pragma once

class StructuredBuffer
{
public:
	StructuredBuffer();
	~StructuredBuffer();

	static StructuredBuffer* Create(uint32 aTypeSize, uint32 aDataCount, void* someInitalData = nullptr);
	void BindToCS(uint32 aSlot);
	void BindToPS(uint32 aSlot);

private:
	ComPtr<ID3D11Buffer> mStructuredBuffer;
	ComPtr<ID3D11ShaderResourceView> mSRV;
};