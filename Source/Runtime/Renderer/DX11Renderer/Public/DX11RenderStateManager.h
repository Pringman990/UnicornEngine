#pragma once
#include <IRenderStateManager.h>

#include "ConstantBuffer/DX11ConstantBuffer.h"

struct DX11VertexBuffer;
struct DX11IndexBuffer;
class InputLayout;
class DX11PixelShaderBackend;
class DX11VertexShaderBackend;

class DX11ConstantBuffer;

class DX11RenderStateManager : public IRenderStateManager
{
public:
	DX11RenderStateManager();
	~DX11RenderStateManager() override;

	virtual bool Init() override;

	virtual void SetMaterialShader(MaterialShader* aShader) override;
	virtual void SetInputLayoutShader(InputLayout* aLayout) override;
	
	virtual void SetVertexBuffer(RenderBuffer* aVertexBuffer) override;
	virtual void SetIndexBuffer(RenderBuffer* aIndexBuffer) override;
	
	virtual void SetObjectTransform(const Transform& aTransform) override;

	virtual void SetAmbientLight(AmbientLight* aAmbientLight) override;

	virtual void DrawMesh(uint32 aStartIndex, uint32 aIndexCount) override;

	void FinishCommandList() override;

private:
	void UpdateState();
private:
	struct RenderState
	{
		ObjectConstantBufferData objectConstantBufferData = {};
		LightConstantBufferData lightConstantBufferData = {};
		DX11VertexBuffer* vertexBuffer = nullptr;
		DX11IndexBuffer* indexBuffer = nullptr;
		InputLayout* inputLayout = nullptr;
		DX11PixelShaderBackend* pixelShader = nullptr;
		DX11VertexShaderBackend* vertexShader = nullptr;
		AmbientLight* ambientLight = nullptr;
	} mCurrentState;

	RenderState mGPUState;

	ID3D11DeviceContext* mRendererDefferedContext;

	DX11ConstantBuffer* mObjConstant;
	DX11ConstantBuffer* mLightConstant;
};