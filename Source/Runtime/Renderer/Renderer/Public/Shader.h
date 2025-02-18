#pragma once
#include <StandardTypes/StandardTypes.h>

struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;

class Shader
{
public:
	Shader();
	~Shader();

	void Bind(D3D_PRIMITIVE_TOPOLOGY aTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	static std::shared_ptr<Shader> CreateDefaultVoxel();
	static std::shared_ptr<Shader> CreateDefaultPolygon();
	static std::shared_ptr<Shader> Create(const std::string& aVSPath, const std::string& aPSPath);
	static std::shared_ptr<Shader> Create(const std::string& aVSPath, const std::string& aPSPath, D3D11_INPUT_ELEMENT_DESC aLayout[], uint32 aLayoutCount);

	ID3D11VertexShader* GetVertexShader();
	ID3D11PixelShader* GetPixelShader();
	ID3D11InputLayout* GetInputLayout();

private:
	ComPtr<ID3D11VertexShader> mVertexShader;
	ComPtr<ID3D11PixelShader> mPixelShader;
	ComPtr<ID3D11InputLayout> mInputLayout;
};