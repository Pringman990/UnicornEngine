#pragma once
#include <d3d11.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Buffer;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;

class Shader
{
public:
	Shader();
	~Shader();

	bool CreateShader(
		const std::wstring& aVertexShaderFileName, 
		const std::wstring& aPixelShaderFileName, 
		const std::string& aVertexEntryPoint = "main", 
		const std::string& aVertexShaderModel = "vs_5_0",
		const std::string& aPixelEntryPoint = "main", 
		const std::string& aPixelShaderModel = "ps_5_0"
	);

protected:
	bool PrepareRender(D3D_PRIMITIVE_TOPOLOGY aPrimitiveTopology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST) const;
	virtual bool CreateInputLayout(ID3DBlob* aVertexBlob);
	virtual bool Init() = 0;
private:
	bool LoadVertexShader(const char* aPath, std::string& aData);
	bool LoadPixelShader(const char* aPath);
protected:
	ComPtr<ID3D11VertexShader> mVertexShader;
	ComPtr<ID3D11PixelShader> mPixelShader;
	ComPtr<ID3D11InputLayout> mInputLayout;
};