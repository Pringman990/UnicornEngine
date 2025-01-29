#include "pch.h"
#include "Shader.h"

#include <fstream>

Shader::Shader()
{
}

Shader::~Shader()
{
	mVertexShader.Reset();
	mPixelShader.Reset();
	mInputLayout.Reset();
}

std::shared_ptr<Shader> Shader::CreateDefault()
{
	Renderer* renderer = Renderer::GetInstance();
	ID3D11Device* device = renderer->GetDevice();

	std::shared_ptr<Shader> shader = std::make_shared<Shader>();
	std::string vsData;
	{
		//Load Vertex Shader
		std::ifstream vsFile;
		vsFile.open("../../Binaries/Shaders/Lit_Model_VS.cso", std::ios::binary);
		vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
		HRESULT result = device->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &shader->mVertexShader);
		if (FAILED(result))
		{
			_LOG_RENDERER_WARNING("Failed to setup Device, {}", result);
			return nullptr;
		}
		vsFile.close();

		//Load Pixel Shader
		std::ifstream psFile;
		psFile.open("../../Binaries/Shaders/Lit_Model_PS.cso", std::ios::binary);
		std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
		result = device->CreatePixelShader(psData.data(), psData.size(), nullptr, &shader->mPixelShader);
		if (FAILED(result))
		{
			_LOG_RENDERER_WARNING("Failed to setup Device, {}", result);
			return nullptr;
		}
		psFile.close();
	}

	{
		// Create input layout
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		HRESULT result = device->CreateInputLayout(layout, 2, vsData.data(), vsData.size(), &shader->mInputLayout);
		if (FAILED(result))
		{
			_LOG_RENDERER_WARNING("Failed to setup Device, {}", result);
			return nullptr;
		}
	}
	return shader;
}

std::shared_ptr<Shader> Shader::Create(const std::string& aVSPath, const std::string& aPSPath)
{
	Renderer* renderer = Renderer::GetInstance();
	ID3D11Device* device = renderer->GetDevice();

	std::shared_ptr<Shader> shader = std::make_shared<Shader>();
	std::string vsData;
	{
		//Load Vertex Shader
		std::ifstream vsFile;
		vsFile.open(aVSPath, std::ios::binary);
		vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
		HRESULT result = device->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &shader->mVertexShader);
		if (FAILED(result))
		{
			_LOG_RENDERER_WARNING("Failed to setup Device, {}", result);
			return nullptr;
		}
		vsFile.close();

		//Load Pixel Shader
		std::ifstream psFile;
		psFile.open(aPSPath, std::ios::binary);
		std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
		result = device->CreatePixelShader(psData.data(), psData.size(), nullptr, &shader->mPixelShader);
		if (FAILED(result))
		{
			_LOG_RENDERER_WARNING("Failed to setup Device, {}", result);
			return nullptr;
		}
		psFile.close();
	}

	{
		// Create input layout
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		HRESULT result = device->CreateInputLayout(layout, 2, vsData.data(), vsData.size(), &shader->mInputLayout);
		if (FAILED(result))
		{
			_LOG_RENDERER_WARNING("Failed to setup Device, {}", result);
			return nullptr;
		}
	}
	return shader;
}

ID3D11VertexShader* Shader::GetVertexShader()
{
	return mVertexShader.Get();
}

ID3D11PixelShader* Shader::GetPixelShader()
{
	return mPixelShader.Get();
}

ID3D11InputLayout* Shader::GetInputLayout()
{
	return mInputLayout.Get();
}
