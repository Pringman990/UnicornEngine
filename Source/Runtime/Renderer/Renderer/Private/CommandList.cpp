#include "CommandList.h"

#include "LogicalDevice.h"
#include "Renderer.h"
#include "TextureManager.h"
#include "ShaderManager.h"

#include <d3d11.h>

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

void CommandList::BindConstantBuffer(DirectResourceHandle<GPUConstantBuffer> Buffer, uint32 Slot, ShaderStage Stages) const
{
	if (!Buffer || Buffer.ptr == nullptr || Buffer.ptr->buffer == nullptr)
	{
		LOG_ERROR("Failed to bind constant buffer");
		return;
	}

	if (HasFlag(Stages, ShaderStage::Undefined))
	{
		LOG_ERROR("Can't bind constant buffer to non implemented shader stage");
	}

	if (HasFlag(Stages, ShaderStage::VS))
	{
		mContext->VSSetConstantBuffers(Slot, 1, Buffer.ptr->buffer.GetAddressOf());
	}
	if (HasFlag(Stages, ShaderStage::FS))
	{
		mContext->PSSetConstantBuffers(Slot, 1, Buffer.ptr->buffer.GetAddressOf());
	}
}

void CommandList::BindConstantBuffers(const Vector<DirectResourceHandle<GPUConstantBuffer>>& Buffers, uint32 StartSlot, ShaderStage Stages) const
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

	if (HasFlag(Stages, ShaderStage::VS))
	{
		mContext->VSSetConstantBuffers(StartSlot, static_cast<uint32>(dxBuffers.size()), dxBuffers.data());
	}
	if (HasFlag(Stages, ShaderStage::FS))
	{
		mContext->PSSetConstantBuffers(StartSlot, static_cast<uint32>(dxBuffers.size()), dxBuffers.data());
	}
}

void CommandList::SetRenderTargets(const Vector<GPUResourceHandle<GPUTexture>>& Targets, GPUResourceHandle<GPUTexture> DepthStencil) const
{
	TextureManager* texManager = mRenderer->GetTextureManager();

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
	GPUTexture* rtv = mRenderer->GetTextureManager()->GetInternalTexture(Target);
	mContext->ClearRenderTargetView(rtv->rtv.Get(), ClearColor);
}

void CommandList::ClearDepthStencil(GPUResourceHandle<GPUTexture> Target) const
{
	GPUTexture* dsv = mRenderer->GetTextureManager()->GetInternalTexture(Target);
	mContext->ClearDepthStencilView(dsv->dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void CommandList::SetViewport(GPUResourceHandle<GPUTexture> Viewport) const
{
	GPUTexture* texture = mRenderer->GetTextureManager()->GetInternalTexture(Viewport);
	mContext->RSSetViewports(1, &texture->viewport);
}

void CommandList::SetShader(GPUResourceHandle<Shader> ShaderHandle) const
{
	Shader* shader = mRenderer->GetShaderManager()->GetInternalShader(ShaderHandle);
	mContext->IASetInputLayout(shader->layout->layout.Get());
	mContext->VSSetShader(shader->vs.Get(), nullptr, 0);
	mContext->PSSetShader(shader->ps.Get(), nullptr, 0);
}

void CommandList::SetShaderResources(const Vector<GPUResourceHandle<GPUTexture>>& Textures, uint32 StartSlot, ShaderStage Stages) const
{
	TextureManager* texManager = mRenderer->GetTextureManager();

	Vector<ID3D11ShaderResourceView*> srvs;
	for (uint32 i = 0; i < Textures.size(); i++)
	{
		GPUTexture* srv = texManager->GetInternalTexture(Textures[i]);
		if (srv->srv == nullptr)
		{
			LOG_ERROR("Can't set rendertarget");
		}
		srvs.push_back(srv->srv.Get());
	}

	if (HasFlag(Stages, ShaderStage::VS))
	{
		mContext->VSSetShaderResources(StartSlot, static_cast<uint32>(srvs.size()), srvs.data());
	}
	if (HasFlag(Stages, ShaderStage::FS))
	{
		mContext->PSSetShaderResources(StartSlot, static_cast<uint32>(srvs.size()), srvs.data());
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
