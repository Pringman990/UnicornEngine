#pragma once

//struct ID3DBlob;

class ShaderManager
{
public:
	ShaderManager();
	~ShaderManager();

	ID3D11VertexShader* TryGetVertexShader(const std::wstring& aShaderFileName, ID3DBlob*& aBlob,const std::string& aEntryPoint = "main", const std::string& aShaderModel = "vs_5_0");
	ID3D11PixelShader* TryGetPixelShader(const std::wstring& aShaderFileName, const std::string& aEntryPoint = "main", const std::string& aShaderModel = "ps_5_0");

	bool RecompileVertexShader(const std::wstring& aShaderFileName, const std::string& aEntryPoint = "main", const std::string& aShaderModel = "vs_5_0");
	bool RecompilePixelShader(const std::wstring& aShaderFileName, const std::string& aEntryPoint = "main", const std::string& aShaderModel = "ps_5_0");

private:
	HRESULT CompileShaderFromFile(LPCWSTR srcFile, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** ppBlobOut);
private:
	std::unordered_map<std::wstring, ID3D11VertexShader*> mCompiledVertexShaders;
	std::unordered_map<std::wstring, ID3D11PixelShader*> mCompiledPixelShaders;
};