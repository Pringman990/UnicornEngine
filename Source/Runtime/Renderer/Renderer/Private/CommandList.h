#pragma once
#include <EngineMinimal.h>
#include "RendererMinimal.h"

#include "GPUResources/GPUBuffer.h"
#include "GPUResources/GPUTexture.h"
#include "GPUResources/GPUMaterial.h"
#include "GPUResources/GPUMesh.h"
#include "Shader.h"
#include "Sampler.h"

class LogicalDevice;
class Renderer;

class Material;
class Mesh;

struct ID3D11CommandList;

class CommandList
{
	friend class Renderer;
	friend class LogicalDevice;
public:
	struct RenderCommand
	{
		enum class CommandType
		{
			DrawMesh
		};

		struct DrawSubMesh
		{
			GPUResourceHandle<GPUMesh> mesh;
			GPUResourceHandle<GPUMaterial> material;
			Transform transform;
			uint32 startIndex;
			uint32 indexCount;
			uint32 renderID = 0;
		};

		CommandType type;

		Variant<
			DrawSubMesh
		> data;
	};

	struct MapContext
	{
		void* data;
		uint32 rowPitch;
		uint32 depthPitch;
	};

public:
	CommandList();
	CommandList(ComPtr<ID3D11DeviceContext> DefferedContext, Renderer* InRenderer);
	~CommandList();

	//High Level

	void DrawMesh(Mesh* MeshAsset, const Transform& MeshTransform);
	void DrawMesh(Mesh* MeshAsset, const Transform& MeshTransform, uint32 RenderID);

	ID3D11CommandList* Finalize();

	//Low Level
	void SetSamplers(const Vector<Sampler*>& Samplers, uint32 StartSlot);

	void SetTopology(PrimitiveTopology Topology) const;
	void SetVertexBuffer(DirectResourceHandle<GPUBuffer> Buffer, uint32 Stride) const;
	void SetIndexBuffer(DirectResourceHandle<GPUBuffer> Buffer, RenderFormat Format = RenderFormat::R32_UINT) const;

	void UpdateConstantBuffer(DirectResourceHandle<GPUConstantBuffer> Buffer, void* Data) const;
	void BindConstantBuffer(DirectResourceHandle<GPUConstantBuffer> Buffer, uint32 Slot, ShaderStageBind Stages) const;
	void BindConstantBuffers(const Vector<DirectResourceHandle<GPUConstantBuffer>>& Buffers, uint32 StartSlot, ShaderStageBind Stages) const;

	void SetRenderTargets(const Vector<GPUResourceHandle<GPUTexture>>& Targets, GPUResourceHandle<GPUTexture> DepthStencil) const;

	void ClearRenderTarget(GPUResourceHandle<GPUTexture> Target, Color ClearColor) const;
	void ClearDepthStencil(GPUResourceHandle<GPUTexture> Target) const;

	void SetViewport(GPUResourceHandle<GPUTexture> Viewport) const;

	void SetShaderProgram(GPUResourceHandle<ShaderProgram> ShaderHandle) const;

	void SetShaderResources(const Vector<GPUResourceHandle<GPUTexture>>& Textures, uint32 StartSlot, ShaderStageBind Stages) const;
	void SetShaderResource(GPUResourceHandle<GPUTexture> Texture, uint32 Slot, ShaderStageBind Stages) const;

	void DrawIndexed(uint32 IndexCount, uint32 StartIndex) const;

	ID3D11CommandList* Finish();
	void Reset();

	void CopyResource(GPUResourceHandle<GPUTexture> From, GPUResourceHandle<GPUTexture> To);
	void Map(GPUResourceHandle<GPUTexture> Texture, MapContext& Context);
	void Unmap(GPUResourceHandle<GPUTexture> Texture);

private:
private:
	Renderer* mRenderer;
	ComPtr<ID3D11DeviceContext> mContext;
	ComPtr<ID3D11CommandList> mCachedList;

	Vector<RenderCommand> mCommandQueue;
};