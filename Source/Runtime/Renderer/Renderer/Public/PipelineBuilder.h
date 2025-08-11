#pragma once
#include <Core.h>

#include "Pipeline.h"
#include "VertexLayout.h"

class PipelineBuilder
{
public:
	PipelineBuilder();
	~PipelineBuilder();

	PipelineBuilder& SetShaders(const Vector<VkPipelineShaderStageCreateInfo>& ShaderStage);
	PipelineBuilder& SetComputeShader(const VkPipelineShaderStageCreateInfo& ShaderStage);
	PipelineBuilder& SetVertexLayout(VertexLayoutType Type);
	PipelineBuilder& SetInputAssembly(VkPrimitiveTopology Topology);
	PipelineBuilder& SetRasterizer(VkCullModeFlags CullMode, VkPolygonMode PolygonMode = VK_POLYGON_MODE_FILL, float LineWidth = 1.0f);
	PipelineBuilder& SetDefaultBlendStates(uint32 ColorAttachmentCount);
	PipelineBuilder& SetDepthTest(bool TestEnable, bool WriteEnable = true, VkCompareOp CompareOp = VK_COMPARE_OP_LESS);
	PipelineBuilder& SetDefaultMultisampling();
	PipelineBuilder& SetDynamicStates(const Vector<VkDynamicState>& States);
	PipelineBuilder& SetViewportState(const VkViewport& Viewport, const VkRect2D& Scissor);
	PipelineBuilder& SetRenderingInfo(const VkFormat* colorFormats, uint32 colorCount, VkFormat depthFormat, VkFormat stencilFormat);
	PipelineBuilder& SetDescriptorSetLayouts(const Vector<VkDescriptorSetLayout>& Layouts);

	//Good for split screen
	PipelineBuilder& SetViewportState(const Vector<VkViewport>& Viewports, const Vector<VkRect2D>& Scissors);


	Pipeline* Build();

private:
	struct PipelineBuildInfo
	{
		Vector<VkPipelineShaderStageCreateInfo> shaderStages;//
		Vector<VkDescriptorSetLayout> descriptorSetLayouts;
		Vector<VkPushConstantRange> pushConstants;
		Vector<VkDynamicState> dynamicStates;
		Vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;//

		VkPipelineDynamicStateCreateInfo dynamicState{};//
		
		VertexLayout vertexLayout{};//
		bool useVertexLayout = false;

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};//
		
		VkPipelineViewportStateCreateInfo viewportState{};//
		bool useFixedViewport = false;

		VkPipelineRasterizationStateCreateInfo rasterizer{};//
		VkPipelineMultisampleStateCreateInfo multisampleState{};//
		VkPipelineRenderingCreateInfoKHR renderingInfo{};//

		VkPipelineDepthStencilStateCreateInfo depthStencil{};//
		bool useDepthStencil = false;

		enum class ShaderStage
		{
			Rasterizer,
			Compute
		}shaderStage;

	} mBuildInfo;
};