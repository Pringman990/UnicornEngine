#include "CommandList.h"

#include "LogicalDevice.h"
#include "Renderer.h"
#include "GPUResources/GPUTextureManager.h"
#include "GPUResources/GPUMeshManager.h"
#include "GPUResources/GPUMaterialManager.h"
#include "ShaderManager.h"

#include <d3d11.h>

#include "RenderAssets/Material.h"
#include "RenderAssets/Mesh.h"

CommandList::CommandList(ComPtr<ID3D11DeviceContext> DefferedContext, Renderer* InRenderer)
	:
	mRenderer(InRenderer),
	mContext(DefferedContext)
{

}

CommandList::CommandList()
	:
	mRenderer(nullptr),
	mContext(nullptr)
{
}

CommandList::~CommandList()
{
	mContext->ClearState();
	mContext->Flush();
}


void CommandList::DrawMesh(Mesh* MeshAsset, const Transform& MeshTransform)
{
	for (uint32 i = 0; i < MeshAsset->GetSubmeshes().size(); i++)
	{
		const auto& submesh = MeshAsset->GetSubmeshes()[i];

		RenderCommand::DrawSubMesh drawMeshCmd;
		drawMeshCmd.mesh = MeshAsset->GetGPUMeshHandle();
		drawMeshCmd.transform = MeshTransform;
		drawMeshCmd.startIndex = submesh.startIndex;
		drawMeshCmd.indexCount = submesh.indexCount;

		if (!submesh.material)
		{
			//Set to invalid material
			drawMeshCmd.material = mRenderer->GetGPUMaterialManager()->GetInvalid();
		}
		else
		{
			if (submesh.material->IsDirty())
			{
				mRenderer->GetGPUMaterialManager()->UpdateFromAsset(submesh.material->GetGPUMaterialHandle(), submesh.material.Get());
			}
			drawMeshCmd.material = submesh.material->GetGPUMaterialHandle();
		}


		RenderCommand cmd;
		cmd.type = RenderCommand::CommandType::DrawMesh;
		cmd.data = drawMeshCmd;

		mCommandQueue.push_back(std::move(cmd));
	}
}

ID3D11CommandList* CommandList::Finalize()
{
	for (const auto& cmd : mCommandQueue)
	{
		switch (cmd.type)
		{
		case RenderCommand::CommandType::DrawMesh:
		{
			auto drawCmd = std::get<RenderCommand::DrawSubMesh>(cmd.data);
			auto meshHandle = drawCmd.mesh;
			GPUMesh* mesh = mRenderer->GetGPUMeshManager()->GetInternalMesh(meshHandle);

			auto objCBuffer = mRenderer->GetObjectConstantBuffer();
			ObjectConstantBufferData objBuffer;
			objBuffer.modelToWorld = drawCmd.transform.GetMatrix();
			UpdateConstantBuffer(objCBuffer, &objBuffer);
			BindConstantBuffer(objCBuffer, (uint32)ConstantBufferBindSlots::Object, ShaderStageBind::FS | ShaderStageBind::VS);

			auto material = mRenderer->GetGPUMaterialManager()->GetInternalMaterial(drawCmd.material);
			SetShaderProgram(material->shaderProgram);
			for (uint32 i = 0; i < material->boundTextures.size(); i++)
			{
				const auto& resource = material->boundTextures[i];
				SetShaderResource(resource.texture, resource.slot, resource.stage);
			}

			SetTopology(PrimitiveTopology::TriangleList);
			SetVertexBuffer(mesh->vertexBuffer, sizeof(Vertex));
			SetIndexBuffer(mesh->indexBuffer);
			DrawIndexed(drawCmd.indexCount, drawCmd.startIndex);

			break;
		}
		default:
			THROW("Trying to process non implemented render command");
			break;
		}
	}

	mCommandQueue.clear();

	return Finish();
}

void CommandList::SetSamplers(const Vector<Sampler*>& Samplers, uint32 StartSlot)
{
	Vector<ID3D11SamplerState*> samplers;
	for (uint32 i = 0; i < Samplers.size(); i++)
	{
		samplers.push_back(Samplers[i]->GetRaw());
	}

	mContext->PSSetSamplers(StartSlot, static_cast<uint32>(samplers.size()), samplers.data());
}

void CommandList::SetTopology(PrimitiveTopology Topology) const
{
	mContext->IASetPrimitiveTopology(ToD11Topology(Topology));
}

void CommandList::SetVertexBuffer(DirectResourceHandle<GPUBuffer> Buffer, uint32 Stride) const
{
	uint32 stride = Stride;
	uint32 offset = 0;
	mContext->IASetVertexBuffers(0, 1, Buffer.ptr->buffer.GetAddressOf(), &stride, &offset);
}

void CommandList::SetIndexBuffer(DirectResourceHandle<GPUBuffer> Buffer, RenderFormat Format) const
{
	mContext->IASetIndexBuffer(Buffer.ptr->buffer.Get(), ToDXFormat(Format), 0);
}

void CommandList::UpdateConstantBuffer(DirectResourceHandle<GPUConstantBuffer> Buffer, void* Data) const
{
	if (!Buffer || Buffer.ptr == nullptr || Buffer.ptr->buffer == nullptr || Data == nullptr)
	{
		LOG_ERROR("Failed to update constant buffer");
		return;
	}

	if (Buffer.ptr->usage == BufferUsage::Default)
	{
		mContext->UpdateSubresource(Buffer.ptr->buffer.Get(), 0, nullptr, Data, 0, 0);
	}
	else if (Buffer.ptr->usage == BufferUsage::Dynamic)
	{
		D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
		mContext->Map(Buffer.ptr->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
		memcpy(mappedBuffer.pData, Data, Buffer.ptr->size);
		mContext->Unmap(Buffer.ptr->buffer.Get(), 0);
	}
	else
	{
		LOG_ERROR("Trying to update constant buffer with non implemented update function");
	}

	memcpy_s(Buffer.ptr->cpuData.data(), Buffer.ptr->size, Data, Buffer.ptr->size);
}

void CommandList::BindConstantBuffer(DirectResourceHandle<GPUConstantBuffer> Buffer, uint32 Slot, ShaderStageBind Stages) const
{
	if (!Buffer || Buffer.ptr == nullptr || Buffer.ptr->buffer == nullptr)
	{
		LOG_ERROR("Failed to bind constant buffer");
		return;
	}

	if (HasFlag(Stages, ShaderStageBind::Undefined))
	{
		LOG_ERROR("Can't bind constant buffer to non implemented shader stage");
	}

	if (HasFlag(Stages, ShaderStageBind::VS))
	{
		mContext->VSSetConstantBuffers(Slot, 1, Buffer.ptr->buffer.GetAddressOf());
	}
	if (HasFlag(Stages, ShaderStageBind::FS))
	{
		mContext->PSSetConstantBuffers(Slot, 1, Buffer.ptr->buffer.GetAddressOf());
	}
}

void CommandList::BindConstantBuffers(const Vector<DirectResourceHandle<GPUConstantBuffer>>& Buffers, uint32 StartSlot, ShaderStageBind Stages) const
{
	if (Buffers.size() == 0)
	{
		LOG_ERROR("Failed to bind constant buffers");
		return;
	}

	Vector<ID3D11Buffer*> dxBuffers;
	for (uint32 i = 0; i < Buffers.size(); i++)
	{
		if (Buffers[i].ptr == nullptr)
		{
			LOG_ERROR("Constant buffer is not vaild, removed from binding");
		}

		dxBuffers.push_back(Buffers[i].ptr->buffer.Get());
	}

	if (HasFlag(Stages, ShaderStageBind::VS))
	{
		mContext->VSSetConstantBuffers(StartSlot, static_cast<uint32>(dxBuffers.size()), dxBuffers.data());
	}
	if (HasFlag(Stages, ShaderStageBind::FS))
	{
		mContext->PSSetConstantBuffers(StartSlot, static_cast<uint32>(dxBuffers.size()), dxBuffers.data());
	}
}

void CommandList::SetRenderTargets(const Vector<GPUResourceHandle<GPUTexture>>& Targets, GPUResourceHandle<GPUTexture> DepthStencil) const
{
	GPUTextureManager* texManager = mRenderer->GetGPUTextureManager();

	Vector<ID3D11RenderTargetView*> rtvs;
	for (uint32 i = 0; i < Targets.size(); i++)
	{
		GPUTexture* rtv = texManager->GetInternalTexture(Targets[i]);
		if (rtv->rtv == nullptr)
		{
			LOG_ERROR("Can't set rendertarget");
		}
		rtvs.push_back(rtv->rtv.Get());
	}

	mContext->OMSetRenderTargets(static_cast<uint32>(Targets.size()), rtvs.data(), texManager->GetInternalTexture(DepthStencil)->dsv.Get());
}

void CommandList::ClearRenderTarget(GPUResourceHandle<GPUTexture> Target, Color ClearColor) const
{
	GPUTexture* rtv = mRenderer->GetGPUTextureManager()->GetInternalTexture(Target);
	mContext->ClearRenderTargetView(rtv->rtv.Get(), ClearColor);
}

void CommandList::ClearDepthStencil(GPUResourceHandle<GPUTexture> Target) const
{
	GPUTexture* dsv = mRenderer->GetGPUTextureManager()->GetInternalTexture(Target);
	mContext->ClearDepthStencilView(dsv->dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void CommandList::SetViewport(GPUResourceHandle<GPUTexture> Viewport) const
{
	GPUTexture* texture = mRenderer->GetGPUTextureManager()->GetInternalTexture(Viewport);
	mContext->RSSetViewports(1, &texture->viewport);
}

void CommandList::SetShaderProgram(GPUResourceHandle<ShaderProgram> ShaderHandle) const
{
	ShaderProgram* shader = mRenderer->GetShaderManager()->GetInternalShaderProgram(ShaderHandle);
	mContext->IASetInputLayout(shader->inputLayout->layout.Get());
	mContext->VSSetShader(static_cast<ID3D11VertexShader*>(shader->vs.shader.Get()), nullptr, 0);
	mContext->PSSetShader(static_cast<ID3D11PixelShader*>(shader->fs.shader.Get()), nullptr, 0);
}

void CommandList::SetShaderResources(const Vector<GPUResourceHandle<GPUTexture>>& Textures, uint32 StartSlot, ShaderStageBind Stages) const
{
	GPUTextureManager* texManager = mRenderer->GetGPUTextureManager();

	Vector<ID3D11ShaderResourceView*> srvs;
	for (uint32 i = 0; i < Textures.size(); i++)
	{
		if (!Textures[i])
		{
			LOG_WARNING("Handle invalid can't set srv");
			continue;
		}

		GPUTexture* srv = texManager->GetInternalTexture(Textures[i]);
		if (srv->srv == nullptr)
		{
			LOG_ERROR("Srv invalid");
			continue;
		}
		srvs.push_back(srv->srv.Get());
	}

	if (HasFlag(Stages, ShaderStageBind::VS))
	{
		mContext->VSSetShaderResources(StartSlot, static_cast<uint32>(srvs.size()), srvs.data());
	}
	if (HasFlag(Stages, ShaderStageBind::FS))
	{
		mContext->PSSetShaderResources(StartSlot, static_cast<uint32>(srvs.size()), srvs.data());
	}
}

void CommandList::SetShaderResource(GPUResourceHandle<GPUTexture> Texture, uint32 Slot, ShaderStageBind Stages) const
{
	if (!Texture)
	{
		static ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
		if (HasFlag(Stages, ShaderStageBind::VS))
		{
			mContext->VSSetShaderResources(Slot, 1, nullSRV);
		}
		if (HasFlag(Stages, ShaderStageBind::FS))
		{
			mContext->PSSetShaderResources(Slot, 1, nullSRV);
		}
		return;
	}

	GPUTextureManager* texManager = mRenderer->GetGPUTextureManager();
	GPUTexture* srv = texManager->GetInternalTexture(Texture);
	if (srv == nullptr)
	{
		static ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
		if (HasFlag(Stages, ShaderStageBind::VS))
		{
			mContext->VSSetShaderResources(Slot, 1, nullSRV);
		}
		if (HasFlag(Stages, ShaderStageBind::FS))
		{
			mContext->PSSetShaderResources(Slot, 1, nullSRV);
		}
		return;
	}

	if (HasFlag(Stages, ShaderStageBind::VS))
	{
		mContext->VSSetShaderResources(Slot, 1, srv->srv.GetAddressOf());
	}
	if (HasFlag(Stages, ShaderStageBind::FS))
	{
		mContext->PSSetShaderResources(Slot, 1, srv->srv.GetAddressOf());
	}
}

void CommandList::DrawIndexed(uint32 IndexCount, uint32 StartIndex) const
{
	mContext->DrawIndexed(IndexCount, StartIndex, 0);
}

ID3D11CommandList* CommandList::Finish()
{
	HRESULT hr = mContext->FinishCommandList(FALSE, mCachedList.ReleaseAndGetAddressOf());
	if (FAILED(hr))
	{
		LOG_ERROR("Failed to finish commandlist, {}", hr);
	}

	return mCachedList.Get();
}

void CommandList::Reset()
{
	mCachedList.Reset();
}
