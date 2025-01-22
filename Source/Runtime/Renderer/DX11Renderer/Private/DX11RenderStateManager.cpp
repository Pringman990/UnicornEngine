#include "pch.h"
#include "DX11RenderStateManager.h"

#include "Shader/DX11PixelShaderBackend.h"
#include "Shader/DX11VertexShaderBackend.h"
#include "Shader/DX11InputLayout.h"
#include "Buffers/DX11Buffers.h"

DX11RenderStateManager::DX11RenderStateManager()
	:
	mGPUState({}),
	mRendererDefferedContext(nullptr),
	mObjConstant(nullptr),
	mLightConstant(nullptr)
{
}

DX11RenderStateManager::~DX11RenderStateManager()
{
	delete mObjConstant;
	mObjConstant = nullptr;

	delete mLightConstant;
	mLightConstant = nullptr;
	
}

bool DX11RenderStateManager::Init()
{
	mRendererDefferedContext = DX11RenderingBackend::_GetInstance()->GetDeviceContext();
	_ENSURE_RENDERER(mRendererDefferedContext, "Device Context is nullptr");
	
	mObjConstant = new DX11ConstantBuffer();
	mCurrentState.objectConstantBufferData.modelToWorld = Matrix();
	mObjConstant->Init(sizeof(ObjectConstantBufferData), &mCurrentState.objectConstantBufferData);
	
	mLightConstant = new DX11ConstantBuffer();
	mLightConstant->Init(sizeof(LightConstantBufferData), &mCurrentState.lightConstantBufferData);

	return true;
}

void DX11RenderStateManager::SetMaterialShader(MaterialShader* aShader)
{
	mCurrentState.vertexShader = static_cast<DX11VertexShaderBackend*>(aShader->GetVertexShader());
	mCurrentState.pixelShader = static_cast<DX11PixelShaderBackend*>(aShader->GetPixelShader());
}

void DX11RenderStateManager::SetInputLayoutShader(InputLayout* aLayout)
{
	mCurrentState.inputLayout = aLayout;
}

void DX11RenderStateManager::SetVertexBuffer(RenderBuffer* aVertexBuffer)
{
	mCurrentState.vertexBuffer = static_cast<DX11VertexBuffer*>(aVertexBuffer);
}

void DX11RenderStateManager::SetIndexBuffer(RenderBuffer* aIndexBuffer)
{
	mCurrentState.indexBuffer = static_cast<DX11IndexBuffer*>(aIndexBuffer);
}

void DX11RenderStateManager::SetObjectTransform(const Transform& aTransform)
{
	mCurrentState.objectConstantBufferData.modelToWorld = aTransform.GetMatrix();
}

void DX11RenderStateManager::SetAmbientLight(AmbientLight* aAmbientLight)
{
	mCurrentState.lightConstantBufferData.ambientLightColorAndIntensity = aAmbientLight->GetSolidColor();

	mCurrentState.ambientLight = aAmbientLight;
}

void DX11RenderStateManager::DrawMesh(uint32 aStartIndex, uint32 aIndexCount)
{
	UpdateState();
	mRendererDefferedContext->DrawIndexed(aIndexCount, aStartIndex, 0);
}

void DX11RenderStateManager::FinishCommandList()
{
	//if (!mRendererDefferedContext)
	//	return;

	//ID3D11CommandList* commandList = nullptr;
	//HRESULT hr = mRendererDefferedContext->FinishCommandList(FALSE, &commandList);
	//if (FAILED(hr)) {
	//	// Handle error (log, assert, etc.)
	//	return;
	//}
	//if (!commandList)
	//	return;

	//// Execute the command list on the immediate context (main thread)
	//DX11RenderingBackend::_GetInstance()->GetDeviceContext()->ExecuteCommandList(commandList, FALSE);

	//// Release the command list after execution
	//commandList->Release();

	//mRendererDefferedContext->ClearState();
}

void DX11RenderStateManager::UpdateState()
{
	if (mCurrentState.pixelShader != mGPUState.pixelShader)
	{
		mCurrentState.pixelShader->Bind();
	}

	if (mCurrentState.vertexShader != mGPUState.vertexShader)
	{
		mCurrentState.vertexShader->Bind();
	}

	if (mCurrentState.inputLayout != mGPUState.inputLayout)
	{
		mCurrentState.inputLayout->Bind();
	}

	if (mCurrentState.vertexBuffer != mGPUState.vertexBuffer)
	{
		uint32_t stride = sizeof(Vertex);
		uint32_t offset = 0;
		mRendererDefferedContext->IASetVertexBuffers(0, 1, &mCurrentState.vertexBuffer->mVertexBuffer, &stride, &offset);
	}

	if (mCurrentState.indexBuffer != mGPUState.indexBuffer)
	{
		mRendererDefferedContext->IASetIndexBuffer(mCurrentState.indexBuffer->mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	}

	if (mCurrentState.objectConstantBufferData != mGPUState.objectConstantBufferData)
	{
		mObjConstant->UpdateData(&mCurrentState.objectConstantBufferData);
		mObjConstant->Bind(ConstantBuffers::eObjectConstantBuffer);
	}

	if (mCurrentState.ambientLight != mGPUState.ambientLight || mCurrentState.lightConstantBufferData != mGPUState.lightConstantBufferData)
	{
		if (mCurrentState.ambientLight->GetCubemap())
		{
			mCurrentState.ambientLight->GetCubemap()->Bind(0);
			mCurrentState.lightConstantBufferData.isUsingCubemap = true;
		}
		else
		{
			mCurrentState.lightConstantBufferData.isUsingCubemap = false;
		}

		mLightConstant->UpdateData(&mCurrentState.lightConstantBufferData);
		mLightConstant->Bind(ConstantBuffers::eLightConstantBuffer);
	}

	mGPUState = mCurrentState;
}
