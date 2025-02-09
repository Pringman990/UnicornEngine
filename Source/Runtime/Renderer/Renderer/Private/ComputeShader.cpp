#include "pch.h"
#include "ComputeShader.h"

#include "Texture2D.h"

ComputeShader::ComputeShader()
{
}

ComputeShader::~ComputeShader()
{
	mShader.Reset();
}

bool ComputeShader::Init(
	const std::string& aShaderPath
)
{
	ID3D11Device* device = Renderer::GetInstance()->GetDevice();

	std::ifstream csFile;
	csFile.open(aShaderPath, std::ios::binary);
	if (!csFile.is_open()) {
		_LOG_RENDERER_ERROR("Failed to open compute shader file: {}", aShaderPath);
		return false;
	}

	std::string csData = { std::istreambuf_iterator<char>(csFile), std::istreambuf_iterator<char>() };
	_LOG_RENDERER_INFO("Loaded shader file with size: {}", csData.size());

	HRESULT hr = device->CreateComputeShader(csData.data(), csData.size(), nullptr, mShader.GetAddressOf());
	if (FAILED(hr))
	{
		_LOG_CORE_ERROR("Failed to create computeShader: {}", hr);
		return false;
	}

	return true;
}

void ComputeShader::Dispatch(
	const uint32& aThreadGroupSizeX, 
	const uint32& aThreadGroupSizeY, 
	const uint32& aThreadGroupSizeZ
)
{
	ID3D11DeviceContext* context = Renderer::GetInstance()->GetDeviceContext();
	context->CSSetShader(mShader.Get(), nullptr, 0);

	context->Dispatch(aThreadGroupSizeX, aThreadGroupSizeY, aThreadGroupSizeZ);
}
