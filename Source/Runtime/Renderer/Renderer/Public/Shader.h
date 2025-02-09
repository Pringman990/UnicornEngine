#pragma once

struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;

class Shader
{
public:
	Shader();
	~Shader();

	void Bind();

	static std::shared_ptr<Shader> CreateDefaultVoxel();
	static std::shared_ptr<Shader> CreateDefaultPolygon();
	static std::shared_ptr<Shader> Create(const std::string& aVSPath, const std::string& aPSPath);

	ID3D11VertexShader* GetVertexShader();
	ID3D11PixelShader* GetPixelShader();
	ID3D11InputLayout* GetInputLayout();

private:
	ComPtr<ID3D11VertexShader> mVertexShader;
	ComPtr<ID3D11PixelShader> mPixelShader;
	ComPtr<ID3D11InputLayout> mInputLayout;
};