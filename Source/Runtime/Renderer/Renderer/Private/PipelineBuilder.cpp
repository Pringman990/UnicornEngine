#include "PipelineBuilder.h"

PipelineBuilder::PipelineBuilder()
{
}

PipelineBuilder::~PipelineBuilder()
{
}

PipelineBuilder& PipelineBuilder::SetShaders(const Vector<VkPipelineShaderStageCreateInfo>& ShaderStage)
{
	_ENSURE_RENDERER(mType == PipelineType::Graphics, "Can't set graphics shader on other shader pipeline");

	mType = PipelineType::Graphics;

	mBuildInfo.shaderStages = ShaderStage;
	return *this;
}

PipelineBuilder& PipelineBuilder::SetCompute(const VkPipelineShaderStageCreateInfo& ShaderStage)
{
	_ENSURE_RENDERER(mType == PipelineType::Undefined, "Can't set compute shader more the once, or use on graphics shader pipeline");

	mType = PipelineType::Compute;

	mBuildInfo.shaderStages.push_back(ShaderStage);
	return *this;
}

PipelineBuilder& PipelineBuilder::SetVertexLayout(VertexLayoutType Type)
{
	switch (Type)
	{
	case VertexLayoutType::None:
	{
		mBuildInfo.vertexLayout = VertexLayout::GetNullLayout();
		break;
	}
	case VertexLayoutType::Vertex2D:
	{
		mBuildInfo.vertexLayout = VertexLayout::Get2DLayout();
		break;
	}
	case VertexLayoutType::Vertex3D:
	{
		mBuildInfo.vertexLayout = VertexLayout::Get3DLayout();
		break;
	}
	default:
		_LOG_RENDERER_ERROR("Trying to set vertex layout to non implemented layout");
		break;
	}

	mBuildInfo.useVertexLayout = true;
	return *this;
}

PipelineBuilder& PipelineBuilder::SetInputAssembly(VkPrimitiveTopology Topology)
{
	mBuildInfo.inputAssembly = {};
	mBuildInfo.inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	mBuildInfo.inputAssembly.topology = Topology;
	mBuildInfo.inputAssembly.primitiveRestartEnable = VK_FALSE;

	return *this;
}

PipelineBuilder& PipelineBuilder::SetRasterizer(VkCullModeFlags CullMode, VkPolygonMode PolygonMode, float LineWidth)
{
	mBuildInfo.rasterizer = {};
	mBuildInfo.rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	mBuildInfo.rasterizer.depthClampEnable = VK_FALSE;
	mBuildInfo.rasterizer.rasterizerDiscardEnable = VK_FALSE;
	mBuildInfo.rasterizer.polygonMode = PolygonMode;
	mBuildInfo.rasterizer.lineWidth = LineWidth;
	mBuildInfo.rasterizer.cullMode = CullMode;
	mBuildInfo.rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	mBuildInfo.rasterizer.depthBiasEnable = VK_FALSE;
	mBuildInfo.rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	mBuildInfo.rasterizer.depthBiasClamp = 0.0f; // Optional
	mBuildInfo.rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

	return *this;
}

PipelineBuilder& PipelineBuilder::SetDefaultBlendStates(uint32 ColorAttachmentCount)
{
	for (uint32 i = 0; i < ColorAttachmentCount; i++)
	{
		VkPipelineColorBlendAttachmentState createInfo{};
		createInfo.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | 
			VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT;

		createInfo.blendEnable = VK_FALSE;

		mBuildInfo.colorBlendAttachments.push_back(createInfo);
	}

	return *this;
}

PipelineBuilder& PipelineBuilder::SetDepthTest(bool TestEnable, bool WriteEnable, VkCompareOp CompareOp)
{
	mBuildInfo.depthStencil = {}; // Clear any previous settings
	mBuildInfo.depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	mBuildInfo.depthStencil.depthTestEnable = TestEnable;
	mBuildInfo.depthStencil.depthWriteEnable = WriteEnable;
	mBuildInfo.depthStencil.depthCompareOp = CompareOp;

	// Optional stencil — leave off unless needed
	mBuildInfo.depthStencil.stencilTestEnable = VK_FALSE;

	mBuildInfo.useDepthStencil = true;

	return *this;
}

PipelineBuilder& PipelineBuilder::SetDefaultMultisampling()
{
	mBuildInfo.multisampleState = {};
	mBuildInfo.multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	mBuildInfo.multisampleState.sampleShadingEnable = VK_FALSE;
	mBuildInfo.multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	mBuildInfo.multisampleState.minSampleShading = 1.0f; // Optional
	mBuildInfo.multisampleState.pSampleMask = nullptr; // Optional
	mBuildInfo.multisampleState.alphaToCoverageEnable = VK_FALSE; // Optional
	mBuildInfo.multisampleState.alphaToOneEnable = VK_FALSE; // Optional

	return *this;
}

PipelineBuilder& PipelineBuilder::SetDynamicStates(const Vector<VkDynamicState>& States)
{
	mBuildInfo.dynamicState = {};
	mBuildInfo.dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	mBuildInfo.dynamicState.dynamicStateCount = static_cast<uint32>(States.size());
	mBuildInfo.dynamicState.pDynamicStates = States.data();

	return *this;
}

PipelineBuilder& PipelineBuilder::SetViewportState(const VkViewport& Viewport, const VkRect2D& Scissor)
{
	mBuildInfo.viewportState = {};
	mBuildInfo.viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	mBuildInfo.viewportState.scissorCount = 1;
	mBuildInfo.viewportState.pScissors = &Scissor;
	mBuildInfo.viewportState.viewportCount = 1;
	mBuildInfo.viewportState.pViewports = &Viewport;

	mBuildInfo.useFixedViewport = true;

	return *this;
}

PipelineBuilder& PipelineBuilder::SetViewportState(const Vector<VkViewport>& Viewports, const Vector<VkRect2D>& Scissors)
{
	mBuildInfo.viewportState = {};
	mBuildInfo.viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	mBuildInfo.viewportState.scissorCount = static_cast<uint32>(Scissors.size());
	mBuildInfo.viewportState.pScissors = Scissors.data();
	mBuildInfo.viewportState.viewportCount = static_cast<uint32>(Viewports.size());
	mBuildInfo.viewportState.pViewports = Viewports.data();

	mBuildInfo.useFixedViewport = true;

	return *this;
}

PipelineBuilder& PipelineBuilder::SetRenderingInfo(const VkFormat* colorFormats, uint32 colorCount, VkFormat depthFormat, VkFormat stencilFormat)
{
	mBuildInfo.renderingInfo = {};
	mBuildInfo.renderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
	mBuildInfo.renderingInfo.colorAttachmentCount = colorCount;
	mBuildInfo.renderingInfo.pColorAttachmentFormats = colorFormats;
	mBuildInfo.renderingInfo.depthAttachmentFormat = depthFormat;
	mBuildInfo.renderingInfo.stencilAttachmentFormat = stencilFormat;

	return *this;
}

Pipeline* PipelineBuilder::Build()
{
	VkDevice device = Renderer::Get()->GetDevice()->GetRaw();

	Pipeline* pipeline = nullptr;

	switch (mType)
	{
	case PipelineBuilder::Undefined:
		_THROW_RENDERER("Trying to build undefined pipeline");
		break;
	case PipelineBuilder::Graphics:
		pipeline = BuildGraphicsPipeline(device);
		break;
	case PipelineBuilder::Compute:
		pipeline = BuildComputePipeline(device);
		break;
	default:
		_ASSERT_RENDERER(false, "Trying to build non implemented pipeline type");
		break;
	}

	_ENSURE_RENDERER(pipeline, "Pipeline is null after creation");

	return pipeline;
}

Pipeline* PipelineBuilder::BuildGraphicsPipeline(VkDevice Device)
{
	VkPipelineLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	layoutInfo.setLayoutCount = static_cast<uint32>(mBuildInfo.descriptorSetLayouts.size());
	layoutInfo.pSetLayouts = mBuildInfo.descriptorSetLayouts.data();
	layoutInfo.pushConstantRangeCount = static_cast<uint32>(mBuildInfo.pushConstants.size());
	layoutInfo.pPushConstantRanges = mBuildInfo.pushConstants.data();

	VkPipelineLayout layout;
	if (vkCreatePipelineLayout(Device, &layoutInfo, nullptr, &layout) != VK_SUCCESS)
	{
		_THROW_RENDERER("Failed to create pipeline layout");
		return nullptr;
	}

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.attachmentCount = static_cast<uint32>(mBuildInfo.colorBlendAttachments.size());
	colorBlending.pAttachments = mBuildInfo.colorBlendAttachments.data();

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.pNext = &mBuildInfo.renderingInfo;
	pipelineInfo.stageCount = static_cast<uint32>(mBuildInfo.shaderStages.size());
	pipelineInfo.pStages = mBuildInfo.shaderStages.data();

	pipelineInfo.pVertexInputState = mBuildInfo.useVertexLayout ? &mBuildInfo.vertexLayout.vertexInput : VK_NULL_HANDLE;
	pipelineInfo.pInputAssemblyState = &mBuildInfo.inputAssembly;
	pipelineInfo.pRasterizationState = &mBuildInfo.rasterizer;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDepthStencilState = mBuildInfo.useDepthStencil ? &mBuildInfo.depthStencil : VK_NULL_HANDLE;
	pipelineInfo.pDynamicState = mBuildInfo.dynamicState.dynamicStateCount > 0 ? &mBuildInfo.dynamicState : VK_NULL_HANDLE;
	pipelineInfo.pMultisampleState = &mBuildInfo.multisampleState;
	pipelineInfo.pViewportState = mBuildInfo.useFixedViewport ? &mBuildInfo.viewportState : VK_NULL_HANDLE;
	pipelineInfo.layout = layout;

	//We don't use renderpasses from vulkan but instead we use dynamic rendering for simplicity
	pipelineInfo.renderPass = VK_NULL_HANDLE;
	pipelineInfo.subpass = 0;

	VkPipeline vkpipeline;
	if (vkCreateGraphicsPipelines(Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &vkpipeline) != VK_SUCCESS)
	{
		_THROW_RENDERER("Failed to create pipeline");
		return nullptr;
	}

	mBuildInfo = {};

	return new Pipeline(vkpipeline, layout);
}

Pipeline* PipelineBuilder::BuildComputePipeline(VkDevice Device)
{
	VkPipelineLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	layoutInfo.setLayoutCount = static_cast<uint32>(mBuildInfo.descriptorSetLayouts.size());
	layoutInfo.pSetLayouts = mBuildInfo.descriptorSetLayouts.data();
	layoutInfo.pushConstantRangeCount = static_cast<uint32>(mBuildInfo.pushConstants.size());
	layoutInfo.pPushConstantRanges = mBuildInfo.pushConstants.data();

	VkPipelineLayout layout;
	if (vkCreatePipelineLayout(Device, &layoutInfo, nullptr, &layout) != VK_SUCCESS)
	{
		_THROW_RENDERER("Failed to create pipeline layout");
		return nullptr;
	}

	if (mBuildInfo.shaderStages.size() != 1 ||
		mBuildInfo.shaderStages[0].stage != VK_SHADER_STAGE_COMPUTE_BIT)
	{
		_THROW_RENDERER("Compute pipeline must have exactly one compute shader stage");
		return nullptr;
	}

	VkComputePipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	pipelineInfo.stage = mBuildInfo.shaderStages[0];
	pipelineInfo.layout = layout;

	VkPipeline vkpipeline;
	if (vkCreateComputePipelines(Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &vkpipeline) != VK_SUCCESS)
	{
		_THROW_RENDERER("Failed to create pipeline");
		return nullptr;
	}

	mBuildInfo = {};
	
	return new Pipeline(vkpipeline, layout);
}
