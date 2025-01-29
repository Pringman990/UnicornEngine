#include "pch.h"
#include "ComputeShader.h"

#include "Texture.h"

ComputeShader::ComputeShader()
{
}

ComputeShader::~ComputeShader()
{
	mOutputTexture2D.Reset();
	mUAV.Reset();
	mStructuredSRV.Reset();
	mStructuredBuffer.Reset();
	mShader.Reset();
	delete mTexture;
	mTexture = nullptr;
}

bool ComputeShader::Init(
	const std::string& aShaderPath,
	const uint32& aStructuredDataSize,
	const uint32& aStructuredDataCount,
	const uint32& aTextureWidth,
	const uint32& aTextureHeight,
	void* aInitData
)
{
	aInitData;
	ID3D11Device* device = Renderer::GetInstance()->GetDevice();

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = aTextureWidth;
	textureDesc.Height = aTextureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;

	HRESULT hr = device->CreateTexture2D(&textureDesc, nullptr, mOutputTexture2D.GetAddressOf());
	if (FAILED(hr))
	{
		_LOG_RENDERER_ERROR("Failed to create outputTexture: {}", hr);
		return false;
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = textureDesc.Format;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;

	hr = device->CreateUnorderedAccessView(mOutputTexture2D.Get(), &uavDesc, mUAV.GetAddressOf());
	if (FAILED(hr))
	{
		_LOG_RENDERER_ERROR("Failed to create outputUAV: {}", hr);
		return false;
	}

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = aStructuredDataSize * aStructuredDataCount;
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = aStructuredDataSize;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = aInitData;

	hr = device->CreateBuffer(&bufferDesc, &initData, mStructuredBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		_LOG_CORE_ERROR("Failed to create structuredBuffer: {}", hr);
		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.NumElements = aStructuredDataCount;
	srvDesc.Buffer.FirstElement = 0;

	hr = device->CreateShaderResourceView(mStructuredBuffer.Get(), &srvDesc, mStructuredSRV.GetAddressOf());
	if (FAILED(hr))
	{
		_LOG_CORE_ERROR("Failed to create voxelSRV: {}", hr);
		return false;
	}

	mTexture = Texture2D::Create(mOutputTexture2D.Get());

	std::ifstream csFile;
	csFile.open(aShaderPath, std::ios::binary);
	if (!csFile.is_open()) {
		_LOG_RENDERER_ERROR("Failed to open compute shader file: {}", aShaderPath);
		return false;
	}

	std::string csData = { std::istreambuf_iterator<char>(csFile), std::istreambuf_iterator<char>() };
	_LOG_RENDERER_INFO("Loaded shader file with size: {}", csData.size());

	hr = device->CreateComputeShader(csData.data(), csData.size(), nullptr, mShader.GetAddressOf());
	if (FAILED(hr))
	{
		_LOG_CORE_ERROR("Failed to create computeShader: {}", hr);
		return false;
	}

	return true;
}

void ComputeShader::BindAndDispatch(
	const uint32& aThreadGroupSizeX, 
	const uint32& aThreadGroupSizeY, 
	const uint32& aThreadGroupSizeZ
)
{
	ID3D11DeviceContext* context = Renderer::GetInstance()->GetDeviceContext();

	context->CSSetShaderResources(0, 1, mStructuredSRV.GetAddressOf());
	context->CSSetUnorderedAccessViews(0, 1, mUAV.GetAddressOf(), nullptr);
	context->CSSetShader(mShader.Get(), nullptr, 0);

	context->Dispatch(aThreadGroupSizeX, aThreadGroupSizeY, aThreadGroupSizeZ);

	ID3D11UnorderedAccessView* outputUAV = nullptr;
	context->CSSetUnorderedAccessViews(0, 1, &outputUAV, nullptr);
	
	ID3D11ShaderResourceView* srv = nullptr;
	context->CSSetShaderResources(0, 1, &srv);
}

void ComputeShader::UpdateStructuredBuffer(void* someStructuredData)
{
	Renderer::GetInstance()->GetDeviceContext()->UpdateSubresource(mStructuredBuffer.Get(), 0, nullptr, someStructuredData, 0, 0);
}

ID3D11Texture2D* ComputeShader::GetOutputTexture2D()
{
	return mOutputTexture2D.Get();
}
