#include "Vulkan/VkRHIRenderer.h"

#ifdef _Win32
#include <Application/Windows/WindowsApplication.h>
#endif // _Win32

#include <FileSystem/NativeFileBackend.h>

VkRHIRenderer::VkRHIRenderer()
	:
	mDevice(nullptr),
	mInstance(),
	mPhysicalDevice(VK_NULL_HANDLE),
	mSurface(VK_NULL_HANDLE),
	mSwapChain(VK_NULL_HANDLE),
	mSwapChainImageFormat(VK_FORMAT_UNDEFINED),
	mSwapChainExtent({ 0,0 }),
	mCurrentFrameIndex(0)
{
}

VkRHIRenderer::~VkRHIRenderer()
{
	CleanupSwapChain();

	vkDestroyBuffer(*mDevice, mVertexBuffer, nullptr);
	vkFreeMemory(*mDevice, mVertexBufferMemory, nullptr);

	for (uint32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(*mDevice, mImageAvailableSemaphore[i], nullptr);
		vkDestroySemaphore(*mDevice, mRenderFinishedSemaphore[i], nullptr);
		vkDestroyFence(*mDevice, mInFlightFence[i], nullptr);
	}

	vkDestroyCommandPool(*mDevice, mCommandPool, nullptr);

	vkDestroyPipeline(*mDevice, mGraphicsPipeline, nullptr);
	vkDestroyPipelineLayout(*mDevice, mPipelineLayout, nullptr);
	vkDestroyRenderPass(*mDevice, mMainPass, nullptr);

	mDevice.reset(); //We reset here because device must be destroyed before the instance

	vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
	vkDestroyInstance(mInstance, nullptr);
}

bool VkRHIRenderer::Init()
{
	if (!CreateInstance())
		return false;

	if (!CreateSurface())
		return false;

	if (!CreatePhysicalDevice())
		return false;

	mDevice = MakeOwned<VkRHIDevice>(*this);
	if (!mDevice->Create())
		return false;

	if (!CreateSwapChain())
		return false;

	CreateSwapChainImageViews();

	if (!CreateRenderPass())
		return false;

	if (!CreateGraphicsPipeline())
		return false;

	if (!CreateFrameBuffers())
		return false;

	if (!CreateCommandPool())
		return false;

	if (!CreateVertexBuffer())
		return false;

	if (!CreateCommandBuffers())
		return false;

	if (!CreateSyncObjects())
		return false;

	return true;
}

void VkRHIRenderer::PreRender()
{
	vkWaitForFences(*mDevice, 1, &mInFlightFence[mCurrentFrameIndex], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(*mDevice, mSwapChain, UINT64_MAX, mImageAvailableSemaphore[mCurrentFrameIndex], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		ReCreateSwapChain();
		return;
	}

	vkResetFences(*mDevice, 1, &mInFlightFence[mCurrentFrameIndex]);


	vkResetCommandBuffer(mCommandBuffer[mCurrentFrameIndex], 0);

	RecordCommandBuffer(mCommandBuffer[mCurrentFrameIndex], imageIndex);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { mImageAvailableSemaphore[mCurrentFrameIndex] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &mCommandBuffer[mCurrentFrameIndex];

	VkSemaphore signalSemaphores[] = { mRenderFinishedSemaphore[mCurrentFrameIndex] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(mDevice->GetGraphicsQueue(), 1, &submitInfo, mInFlightFence[mCurrentFrameIndex]) != VK_SUCCESS)
	{
		_ENSURE_RENDERER(false, "Failed to submit draw command buffer!");
	}


	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { mSwapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr; // Optional

	result = vkQueuePresentKHR(mDevice->GetPresentQueue(), &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
	{
		ReCreateSwapChain();
	}

	mCurrentFrameIndex = (mCurrentFrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
}

void VkRHIRenderer::Present()
{
}

QueueFamilyIndices VkRHIRenderer::GetQueueFamilies()
{
	_ENSURE_RENDERER(mPhysicalDevice, "Trying to get queue family from a null physical device");
	return FindQueueFamilies(mPhysicalDevice);
}

const Vector<const char*> VkRHIRenderer::GetRequiredDeviceExtensions() const
{
	const Vector<const char*> extensions
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	return extensions;
}

bool VkRHIRenderer::CreateInstance()
{
	VkApplicationInfo appInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
	appInfo.pApplicationName = Application::Get()->GetApplication()->GetWindowInfo().name;
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0); //TODO: Change to actual version, either from premake or config file
	appInfo.pEngineName = "Unicorn Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0); //TODO: Change to actual version, either from premake or config file
	appInfo.apiVersion = VK_API_VERSION_1_0;

	const char* extensions[] =
	{
		VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef _Win32
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif // _Win32
	};

	VkInstanceCreateInfo createInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = _countof(extensions);
	createInfo.ppEnabledExtensionNames = extensions;

	VkResult result = vkCreateInstance(&createInfo, nullptr, &mInstance);
	if (result != VK_SUCCESS)
	{
		_ENSURE_RENDERER(false, "Failed to create Vulkan instance: {}", string_VkResult(result));
		return false;
	}

	return true;
}

bool VkRHIRenderer::CreateSurface()
{
	VkResult result = VK_SUCCESS;

#ifdef _Win32
	WindowsApplication* windowsApp = static_cast<WindowsApplication*>(Application::Get()->GetApplication());
	auto& windowsInfo = windowsApp->GetWindowsWindowInfo();

	VkWin32SurfaceCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hwnd = windowsInfo.windowHandle;
	createInfo.hinstance = windowsInfo.hInstance;

	result = vkCreateWin32SurfaceKHR(mInstance, &createInfo, nullptr, &mSurface);
#else
	assert(false);
#endif // _Win32


	if (result != VK_SUCCESS)
	{
		_ENSURE_RENDERER(false, "Failed to create Vulkan surface: {}", string_VkResult(result));
		return false;
	}

	return true;
}

bool VkRHIRenderer::CreatePhysicalDevice()
{
	uint32 deviceCount = 0;
	VkResult result = vkEnumeratePhysicalDevices(mInstance, &deviceCount, nullptr);
	if (deviceCount == 0 || result != VK_SUCCESS)
	{
		_ENSURE_RENDERER(false, "No supported physical device: {}", string_VkResult(result));
	}

	Vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(mInstance, &deviceCount, devices.data());

	MultiMap<int32, VkPhysicalDevice> candidates;

	for (const auto& device : devices)
	{
		if (IsDeviceSuitable(device))
		{
			int32 score = RateDeviceSuitability(device);
			candidates.insert(std::make_pair(score, device));
		}
	}

	if (candidates.rbegin()->first > 0)
	{
		mPhysicalDevice = candidates.rbegin()->second;
	}
	else
	{
		_ENSURE_RENDERER(false, "No supported physical device even when device(s) exists");
		return false;
	}

	return true;
}

bool VkRHIRenderer::CreateSwapChain()
{
	SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(mPhysicalDevice);
	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = ChooseSwapPresentFormat(swapChainSupport.presentModes);
	VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

	uint32 imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = mSurface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = FindQueueFamilies(mPhysicalDevice);
	uint32_t queueFamilyIndices[] =
	{
		indices.graphicsFamily.value(),
		indices.presentFamily.value()
	};

	if (indices.graphicsFamily != indices.presentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;

	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	VkResult result = vkCreateSwapchainKHR(*mDevice, &createInfo, nullptr, &mSwapChain);
	if (result != VK_SUCCESS)
	{
		_ENSURE_RENDERER(false, "Failed to create SwapChain");
		return false;
	}

	vkGetSwapchainImagesKHR(*mDevice, mSwapChain, &imageCount, nullptr);
	mSwapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(*mDevice, mSwapChain, &imageCount, mSwapChainImages.data());

	mSwapChainImageFormat = surfaceFormat.format;
	mSwapChainExtent = extent;

	return true;
}

bool VkRHIRenderer::CreateGraphicsPipeline()
{
	auto vertShaderCode = FileSystem::ReadAll("shader://Triangle_VS.spv");
	_ENSURE_RENDERER(vertShaderCode.size() % 4 == 0, "Shader: {}, is not aligned to 4 bytes", "Triangle_VS.spv");

	auto fragShaderCode = FileSystem::ReadAll("shader://Triangle_PS.spv");
	_ENSURE_RENDERER(fragShaderCode.size() % 4 == 0, "Shader: {}, is not aligned to 4 bytes", "Triangle_PS.spv");

	VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);
	VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	auto bindingDesc = GetVertexBindingDescription();
	auto attriDesc = GetAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDesc;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32>(attriDesc.size());
	vertexInputInfo.pVertexAttributeDescriptions = &attriDesc[0];

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)mSwapChainExtent.width;
	viewport.height = (float)mSwapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = mSwapChainExtent;

	std::vector<VkDynamicState> dynamicStates =
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;

	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;

	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
		VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT;

	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0; // Optional
	pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

	if (vkCreatePipelineLayout(*mDevice, &pipelineLayoutInfo, nullptr, &mPipelineLayout) != VK_SUCCESS)
	{
		_ENSURE_RENDERER(false, "Failed to create pipeline layout");
		return false;
	}

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;

	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = nullptr; // Optional
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;

	pipelineInfo.layout = mPipelineLayout;
	pipelineInfo.renderPass = mMainPass;
	pipelineInfo.subpass = 0;

	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional

	if (vkCreateGraphicsPipelines(*mDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &mGraphicsPipeline) != VK_SUCCESS) {
		_ENSURE_RENDERER(false, "Failed to create graphics pipeline");
		return false;
	}

	vkDestroyShaderModule(*mDevice, fragShaderModule, nullptr);
	vkDestroyShaderModule(*mDevice, vertShaderModule, nullptr);

	return true;
}

bool VkRHIRenderer::CreateRenderPass()
{
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = mSwapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(*mDevice, &renderPassInfo, nullptr, &mMainPass) != VK_SUCCESS) {
		_ENSURE_RENDERER(false, "Failed to create main render pass");
		return false;
	}

	return true;
}

bool VkRHIRenderer::CreateFrameBuffers()
{
	mSwapChainFramebuffers.resize(mSwapChainImageViews.size());

	for (size_t i = 0; i < mSwapChainImageViews.size(); i++)
	{
		VkImageView attachments[] =
		{
			mSwapChainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = mMainPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = mSwapChainExtent.width;
		framebufferInfo.height = mSwapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(*mDevice, &framebufferInfo, nullptr, &mSwapChainFramebuffers[i]) != VK_SUCCESS)
		{
			_ENSURE_RENDERER(false, "Failed to create frame buffer");
			return false;
		}
	}

	return true;
}

bool VkRHIRenderer::CreateCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(mPhysicalDevice);

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	if (vkCreateCommandPool(*mDevice, &poolInfo, nullptr, &mCommandPool) != VK_SUCCESS)
	{
		_ENSURE_RENDERER(false, "Failed to create command pool");
		return false;
	}

	return true;
}

bool VkRHIRenderer::CreateVertexBuffer()
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = sizeof(vertices[0]) * vertices.size();
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(*mDevice, &bufferInfo, nullptr, &mVertexBuffer) != VK_SUCCESS)
	{
		_ENSURE_RENDERER(false, "Failed to create vertex buffer");
		return false;
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(*mDevice, mVertexBuffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(
		memRequirements.memoryTypeBits,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);

	if (vkAllocateMemory(*mDevice, &allocInfo, nullptr, &mVertexBufferMemory) != VK_SUCCESS) 
	{
		_ENSURE_RENDERER(false, "Failed to allocate vertex buffer");
		return false;
	}

	vkBindBufferMemory(*mDevice, mVertexBuffer, mVertexBufferMemory, 0);

	void* data;
	vkMapMemory(*mDevice, mVertexBufferMemory, 0, bufferInfo.size, 0, &data);
	memcpy(data, vertices.data(), (size_t)bufferInfo.size);
	vkUnmapMemory(*mDevice, mVertexBufferMemory);

	return true;
}

bool VkRHIRenderer::CreateCommandBuffers()
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = mCommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

	if (vkAllocateCommandBuffers(*mDevice, &allocInfo, &mCommandBuffer[0]) != VK_SUCCESS)
	{
		_ENSURE_RENDERER(false, "Failed to create command buffer");
		return false;
	}

	return true;
}

bool VkRHIRenderer::CreateSyncObjects()
{
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (uint32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		if (vkCreateSemaphore(*mDevice, &semaphoreInfo, nullptr, &mImageAvailableSemaphore[i]) != VK_SUCCESS ||
			vkCreateSemaphore(*mDevice, &semaphoreInfo, nullptr, &mRenderFinishedSemaphore[i]) != VK_SUCCESS ||
			vkCreateFence(*mDevice, &fenceInfo, nullptr, &mInFlightFence[i]) != VK_SUCCESS)
		{
			_ENSURE_RENDERER(false, "Failed to create sync objects");
			return false;
		}
	}

	return true;
}

void VkRHIRenderer::ReCreateSwapChain()
{
	vkDeviceWaitIdle(*mDevice);

	CleanupSwapChain();

	CreateSwapChain();
	CreateSwapChainImageViews();
	CreateFrameBuffers();
}
  
void VkRHIRenderer::CleanupSwapChain()
{ 
	for (auto framebuffer : mSwapChainFramebuffers)
	{
		vkDestroyFramebuffer(*mDevice, framebuffer, nullptr);
	}

	for (auto imageView : mSwapChainImageViews)
	{
		vkDestroyImageView(*mDevice, imageView, nullptr);
	}

	vkDestroySwapchainKHR(*mDevice, mSwapChain, nullptr);
}

void VkRHIRenderer::RecordCommandBuffer(VkCommandBuffer Buffer, uint32 ImageIndex)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional

	if (vkBeginCommandBuffer(Buffer, &beginInfo) != VK_SUCCESS)
	{
		_ENSURE_RENDERER(false, "Failed to begin recording command buffer");
		return;
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = mMainPass;
	renderPassInfo.framebuffer = mSwapChainFramebuffers[ImageIndex];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = mSwapChainExtent;

	VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(Buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(Buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mGraphicsPipeline);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(mSwapChainExtent.width);
	viewport.height = static_cast<float>(mSwapChainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(Buffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = mSwapChainExtent;
	vkCmdSetScissor(Buffer, 0, 1, &scissor);

	VkBuffer vertexBuffers[] = { mVertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(Buffer, 0, 1, vertexBuffers, offsets);

	vkCmdDraw(Buffer, static_cast<uint32>(vertices.size()), 1, 0, 0);

	vkCmdEndRenderPass(Buffer);

	if (vkEndCommandBuffer(Buffer) != VK_SUCCESS)
	{
		_ENSURE_RENDERER(false, "Failed to end recording command buffer");
	}
}

void VkRHIRenderer::CreateSwapChainImageViews()
{
	mSwapChainImageViews.resize(mSwapChainImages.size());

	for (size_t i = 0; i < mSwapChainImages.size(); i++)
	{
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = mSwapChainImages[i];

		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = mSwapChainImageFormat;

		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		VkResult result = vkCreateImageView(*mDevice, &createInfo, nullptr, &mSwapChainImageViews[i]);
		if (result != VK_SUCCESS)
		{
			_ENSURE_RENDERER(false, "Failed to create SwapChain image views");
			return;
		}
	}
}

VkShaderModule VkRHIRenderer::CreateShaderModule(const ByteBuffer& ShaderCode)
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = ShaderCode.size();
	createInfo.pCode = reinterpret_cast<const uint32*>(ShaderCode.data());

	VkShaderModule shaderModule;
	VkResult result = vkCreateShaderModule(*mDevice, &createInfo, nullptr, &shaderModule);
	if (result != VK_SUCCESS)
	{
		_ENSURE_RENDERER(false, "Failed to create shader module");
		return nullptr;
	}

	return shaderModule;
}

VkVertexInputBindingDescription VkRHIRenderer::GetVertexBindingDescription()
{
	VkVertexInputBindingDescription desc{};

	desc.binding = 0;
	desc.stride = sizeof(Vertex);
	desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return desc;
}

Array<VkVertexInputAttributeDescription, 2> VkRHIRenderer::GetAttributeDescriptions()
{
	Array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(Vertex, Vertex::position);

	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(Vertex, Vertex::color);

	return attributeDescriptions;
}

uint32 VkRHIRenderer::FindMemoryType(uint32 TypeFilter, VkMemoryPropertyFlags Flags)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) 
	{
		if ((TypeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & Flags) == Flags)
		{
			return i;
		}
	}

	_ENSURE_RENDERER(false, "No available memory type");

	return uint32();
}

bool VkRHIRenderer::IsDeviceSuitable(VkPhysicalDevice Device)
{
	QueueFamilyIndices indices = FindQueueFamilies(Device);

	bool extensionsSupported = CheckDeviceExtensionSupport(Device);

	bool swapChainAdequate = false;
	if (extensionsSupported)
	{
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(Device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

bool VkRHIRenderer::CheckDeviceExtensionSupport(VkPhysicalDevice Device)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(Device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(Device, nullptr, &extensionCount, availableExtensions.data());

	const auto deviceExtensions = GetRequiredDeviceExtensions();

	Set<String> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

int32 VkRHIRenderer::RateDeviceSuitability(VkPhysicalDevice Device)
{
	//Add requirements for choosing a physical device

	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(Device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(Device, &deviceFeatures);

	int32 score = 0;

	// Discrete GPUs have a significant performance advantage
	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
	{
		score += 1000;
	}

	// Maximum possible size of textures affects graphics quality
	score += deviceProperties.limits.maxImageDimension2D;

	// Application can't function without geometry shaders
	if (!deviceFeatures.geometryShader)
	{
		return 0;
	}

	return score;
}

QueueFamilyIndices VkRHIRenderer::FindQueueFamilies(VkPhysicalDevice Device)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(Device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(Device, &queueFamilyCount, queueFamilies.data());



	for (int32 i = 0; i < queueFamilies.size(); i++)
	{
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(Device, i, mSurface, &presentSupport);

		if (presentSupport)
		{
			indices.presentFamily = i;
		}

		if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i;
		}

		if (indices.isComplete())
			break;
	}

	return indices;
}

SwapChainSupportDetails VkRHIRenderer::QuerySwapChainSupport(VkPhysicalDevice Device)
{
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Device, mSurface, &details.capabilities);

	uint32 formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(Device, mSurface, &formatCount, nullptr);

	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(Device, mSurface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(Device, mSurface, &presentModeCount, nullptr);

	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(Device, mSurface, &presentModeCount, details.presentModes.data());
	}



	return details;
}

VkSurfaceFormatKHR VkRHIRenderer::ChooseSwapSurfaceFormat(const Vector<VkSurfaceFormatKHR>& AvailableFormats)
{
	for (const auto& availableFormat : AvailableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
			availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
			)
		{
			return availableFormat;
		}
	}

	return VkSurfaceFormatKHR();
}

VkPresentModeKHR VkRHIRenderer::ChooseSwapPresentFormat(const Vector<VkPresentModeKHR>& AvailablePresentModes)
{
	for (const auto& availablePresentMode : AvailablePresentModes)
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

#undef max
VkExtent2D VkRHIRenderer::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& Capabilites)
{
	if (Capabilites.currentExtent.width != std::numeric_limits<uint32>::max())
	{
		return Capabilites.currentExtent;
	}
	else
	{
		GenericApplication* app = Application::Get()->GetApplication();
		auto& windowInfo = app->GetWindowInfo();

		VkExtent2D actualExtent =
		{
			std::clamp(windowInfo.viewportWidth, Capabilites.minImageExtent.width, Capabilites.maxImageExtent.width),
			std::clamp(windowInfo.viewportHeight, Capabilites.minImageExtent.height, Capabilites.maxImageExtent.height)
		};

		return actualExtent;
	}
}

