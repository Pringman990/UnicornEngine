#pragma once
#include <Core.h>

#include "CommandBuffer.h"
#include "RenderContext.h"

struct RenderScopeInfo
{
	Vector<VkRenderingAttachmentInfo> colorAttachmentInfos;
	VkRenderingInfo renderingInfo{};

	// RenderScopeInfo(VkImageView imageView, VkExtent2D extent, const Color& ClearColor = Color(0.1f, 0.1f, 0.1f, 1.0f))
	RenderScopeInfo(const GPUResourceHandle<GPUTexture>& Handle)
	{
		GPUTexture* texture = Renderer::Get()->GetGPUResourceManager().GetResource(Handle);

		VkRenderingAttachmentInfo colorAttachmentInfo{};
		colorAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		colorAttachmentInfo.imageView = texture->imageView;
		colorAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachmentInfo.resolveMode = VK_RESOLVE_MODE_NONE;
		colorAttachmentInfo.resolveImageView = VK_NULL_HANDLE;
		colorAttachmentInfo.clearValue = VkClearValue{ {0, 0, 0, 0} };

		colorAttachmentInfos.push_back(colorAttachmentInfo);

		renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
		renderingInfo.renderArea.offset = { 0, 0 };
		renderingInfo.renderArea.extent = { texture->extent.width, texture->extent.height };
		renderingInfo.layerCount = 1;
		renderingInfo.viewMask = 0;
		renderingInfo.colorAttachmentCount = static_cast<uint32>(colorAttachmentInfos.size());
		renderingInfo.pColorAttachments = colorAttachmentInfos.data();
		renderingInfo.pDepthAttachment = nullptr;
		renderingInfo.pStencilAttachment = nullptr;
	}

	RenderScopeInfo(const Vector<GPUResourceHandle<GPUTexture>>& Handles)
	{
		_ENSURE_RENDERER(Handles.size() > 0, "Trying to render to no images");

		VkExtent2D extent = {};
		for (uint32 i = 0; i < Handles.size(); i++)
		{
			GPUTexture* texture = Renderer::Get()->GetGPUResourceManager().GetResource(Handles[i]);
			
			//We only use the first image extent as all images must have the same extent
			if (i == 0)
			{
				extent = { texture->extent.width, texture->extent.height };
			}
			else
			{
				_ENSURE_RENDERER(texture->extent.width != extent.width || texture->extent.height != extent.height, "Trying to attach a image with unique extent, all images must have the same extent");
			}

			VkRenderingAttachmentInfo colorAttachmentInfo{};
			colorAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
			colorAttachmentInfo.imageView = texture->imageView;
			colorAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			colorAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			colorAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			colorAttachmentInfo.resolveMode = VK_RESOLVE_MODE_NONE;
			colorAttachmentInfo.resolveImageView = VK_NULL_HANDLE;
			colorAttachmentInfo.clearValue = VkClearValue{ {0, 0, 0, 0} };

			colorAttachmentInfos.push_back(colorAttachmentInfo);
		}

		renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
		renderingInfo.renderArea.offset = { 0, 0 };
		renderingInfo.renderArea.extent = extent;
		renderingInfo.layerCount = 1;
		renderingInfo.viewMask = 0;
		renderingInfo.colorAttachmentCount = static_cast<uint32>(colorAttachmentInfos.size());
		renderingInfo.pColorAttachments = colorAttachmentInfos.data();
		renderingInfo.pDepthAttachment = nullptr;
		renderingInfo.pStencilAttachment = nullptr;
	}
};

class RenderScope
{
public:
	RenderScope(CommandBuffer* Buffer, RenderScopeInfo ScopeInfo);
	RenderScope(RenderScopeInfo ScopeInfo);
	~RenderScope();

private:
	CommandBuffer* mCommandBuffer;
};