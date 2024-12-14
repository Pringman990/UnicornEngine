#pragma once
#include <Shader/IShaderFactory.h>

struct ID3D10Blob;

class DX11ShaderFactory : public IShaderFactory
{
public:
	DX11ShaderFactory();
	~DX11ShaderFactory() override;

	Shader* CreateVertexShader(const std::string& aFileName, const std::string& aEntryPoint, const std::string& aModel) override;
	Shader* CreatePixelShader(const std::string& aFileName, const std::string& aEntryPoint, const std::string& aModel) override;

private:
	HRESULT CompileShaderFromFile(LPCWSTR aSrcFile, LPCSTR aEntryPoint, LPCSTR aShaderModel, ID3D10Blob** ppBlobOut);
private:
};