#pragma once
#include <wrl/client.h>
#include "Light.h"

using Microsoft::WRL::ComPtr;

struct ID3D11ShaderResourceView;

class AmbientLight : public Light
{
public:
	AmbientLight(const Color& aColor, float aIntensity);
	~AmbientLight();

	void SetCubeMap(const std::wstring& aPath);
	ComPtr<ID3D11ShaderResourceView> GetCubeMap();
	uint32_t GetNumMips() const;
private:
	ComPtr<ID3D11ShaderResourceView> mCubemap;
	uint32_t mNumberOfMips;
};