#pragma once

class StructuredBuffer
{
public:
	StructuredBuffer();
	~StructuredBuffer();

	static StructuredBuffer* Create(uint32 aTypeSize, uint32 aDataSize, void* someInitalData = nullptr);
	void Bind(uint32 aSlot);

private:
	ComPtr<ID3D11Buffer> mStructuredBuffer;
	ComPtr<ID3D11ShaderResourceView> mSRV;
};