#include "Renderer.h"
REGISTER_ENGINE_SUBSYSTEM(Renderer)

#include <d3d11.h>

#include "Sampler.h"
#include "SwapChain.h"
#include "CommandList.h"

#include "TextureManager.h"
#include "ShaderManager.h"
#include "InputLayoutManager.h"
#include "RenderBufferManager.h"
#include "MeshManager.h"
#include "MaterialManager.h"

#include "BasicPrimitiveFactory.h"

#include <Application/Windows/WindowsApplication.h>
#include <Application/Windows/WindowsWindowInfo.h>
#include <Application/Application.h>

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
#ifdef _MEMORY_DEBUG
	Microsoft::WRL::ComPtr<ID3D11Debug> debug;
	DX11RenderingBackend::_GetInstance()->GetDevice()->QueryInterface(__uuidof(ID3D11Debug), &debug);
	if (debug) {
		debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	}
#endif
	
}

bool Renderer::Init()
{
	if (!mDevice.Init())
		return false;

	mTextureManager = MakeOwned<TextureManager>(this);
	mShaderManager = MakeOwned<ShaderManager>(this);
	mInputManager = MakeOwned<InputLayoutManager>(this);
	mRenderBufferManager = MakeOwned<RenderBufferManager>(this);
	mMeshManager = MakeOwned<MeshManager>(this);
	mMaterialManager = MakeOwned<MaterialManager>(this);
	
	WindowsApplication* app = static_cast<WindowsApplication*>(SubsystemManager::Get<Application>()->GetApplication());

	DXGI_SWAP_CHAIN_DESC1 swapDesc = {};
	swapDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.Stereo = FALSE;
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
	swapDesc.BufferCount = 2;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	swapDesc.Flags = 0;

	mSwapChain = SwapChain::Create(mDevice, app->GetWindowsWindowInfo().windowHandle, swapDesc);
	if (!mSwapChain)
		THROW("Failed to create swapchain");


	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	mSampler = Sampler::Create(mDevice, samplerDesc);
	if (!mSampler)
		THROW("Failed to create sampler");

	mMaterialManager->Init();

	mFrameSetupCommandList = mDevice.RequestCommandList(this);

	mFrameConstantsBuffer = mRenderBufferManager->CreateConstantBuffer(sizeof(FrameConstantsData), nullptr);
	mObjectConstantBuffer = mRenderBufferManager->CreateConstantBuffer(sizeof(ObjectConstantBufferData), nullptr, BufferUsage::Dynamic);

	return true;
}

void Renderer::SubmitMesh(GPUResourceHandle<GPUMesh> Mesh, const Transform& ObjectTransfrom)
{
	mFrameSetupCommandList->SetTopology(PrimitiveTopology::TriangleList);

	ObjectConstantBufferData objBuffer;
	objBuffer.modelToWorld = ObjectTransfrom.GetMatrix();
	mFrameSetupCommandList->UpdateConstantBuffer(mObjectConstantBuffer, &objBuffer);
	mFrameSetupCommandList->BindConstantBuffer(mObjectConstantBuffer, (uint32)ConstantBufferBindSlots::Object, ShaderStage::FS | ShaderStage::VS);	

	GPUMesh* cubeMesh = GetMeshManager()->GetInternalMesh(Mesh);

	mFrameSetupCommandList->SetVertexBuffer(cubeMesh->vertexBuffer, sizeof(Vertex));
	mFrameSetupCommandList->SetIndexBuffer(cubeMesh->indexBuffer);

	for (auto& subMesh : cubeMesh->submeshes)
	{
		Material* mat = mMaterialManager->GetInternalMaterial(subMesh.materialHandle);
		mFrameSetupCommandList->SetShader(mat->shader);
		mFrameSetupCommandList->DrawIndexed(subMesh.indexCount, subMesh.startIndex);
	}
}

void Renderer::SubmitMesh(GPUResourceHandle<GPUMesh> Mesh, const Transform& ObjectTransfrom, Vector<AssetHandle<Material>> OverrideMaterials)
{
	mFrameSetupCommandList->SetTopology(PrimitiveTopology::TriangleList);

	ObjectConstantBufferData objBuffer;
	objBuffer.modelToWorld = ObjectTransfrom.GetMatrix();
	mFrameSetupCommandList->UpdateConstantBuffer(mObjectConstantBuffer, &objBuffer);
	mFrameSetupCommandList->BindConstantBuffer(mObjectConstantBuffer, (uint32)ConstantBufferBindSlots::Object, ShaderStage::FS | ShaderStage::VS);

	GPUMesh* cubeMesh = GetMeshManager()->GetInternalMesh(Mesh);

	mFrameSetupCommandList->SetVertexBuffer(cubeMesh->vertexBuffer, sizeof(Vertex));
	mFrameSetupCommandList->SetIndexBuffer(cubeMesh->indexBuffer);

	if (OverrideMaterials.size() != cubeMesh->submeshes.size())
	{
		LOG_ERROR("Cancel draw: Override materials {} are not the same size as submesh count {}", OverrideMaterials.size(), cubeMesh->submeshes.size());
		return;
	}

	for (int32 i = 0; i < cubeMesh->submeshes.size(); i++)
	{
		auto& subMesh = cubeMesh->submeshes[i];
		
		AssetHandle<Material> materialHandle;
		if (OverrideMaterials[i])
		{
			materialHandle = OverrideMaterials[i];
		}
		else
		{
			materialHandle = subMesh.materialHandle;
		}
	
		Material* mat = mMaterialManager->GetInternalMaterial(materialHandle);
		mFrameSetupCommandList->SetShader(mat->shader);
		mFrameSetupCommandList->DrawIndexed(subMesh.indexCount, subMesh.startIndex);
	}
}
