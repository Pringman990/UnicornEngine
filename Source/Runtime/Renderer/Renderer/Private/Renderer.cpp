#include "Renderer.h"

#ifdef _Win32
#include <Application/Windows/WindowsApplication.h>
#endif // _Win32

#include <FileSystem/NativeFileBackend.h>

#include "Texture2DLoader.h"
#include "TextureFactory.h"

#include "RenderScope.h"
#include "GPUBarrier.h"

#include "Pipeline.h"
#include "PipelineBuilder.h"
#include "VertexShader.h"
#include "FragmentShader.h"

Renderer::Renderer()
	:
	mInstance(VK_NULL_HANDLE),
	mSurface(nullptr),
	mPhysicalDevice(nullptr),
	mDevice(nullptr),
	mSwapChain(nullptr),
	mActiveCamera(nullptr),
	mCurrentFrameIndex(0)
{
}

Renderer::~Renderer()
{
	AssetManager::Get()->UnRegisterPool<Texture2D>();
	AssetManager::Get()->UnRegisterPool<TextureRenderTarget>();

	vkDeviceWaitIdle(*mDevice);

	for (uint32 i = 0; i < mFrameSyncs.size(); i++)
	{
		mFrameSyncs[i].Destroy();
	}
	mFrameSyncs.clear();

	delete mSwapChain;
	mSwapChain = nullptr;

	delete mDevice;
	mDevice = nullptr;

	delete mSurface;
	mSurface = nullptr;

	delete mPhysicalDevice;
	mPhysicalDevice = nullptr;

	auto funcDestroy = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
		mInstance, "vkDestroyDebugUtilsMessengerEXT");
	if (funcDestroy != nullptr)
	{
		funcDestroy(mInstance, mDebugMessenger, nullptr);
	}

	vkDestroyInstance(mInstance, nullptr);
}

bool Renderer::Init()
{
	AssetManager::Get()->RegisterPool<Texture2D>();
	AssetManager::Get()->RegisterPool<TextureRenderTarget>();

	AssetManager::Get()->RegisterLoader<Texture2D, Texture2DLoader>();

	mGPUResourceManager.RegisterPool<GPUTexture>();

	if (!CreateInstance())
		return false;

	mSurface = Surface::Create(mInstance);
	mPhysicalDevice = PhysicalDevice::Create(mInstance, mSurface);
	mDevice = mPhysicalDevice->CreateLogicalDevice();
	mSwapChain = SwapChain::Create(mPhysicalDevice, mDevice, mSurface);

	mFrameSyncs.reserve(mSwapChain->GetImageCount());
	for (uint32 i = 0; i < mSwapChain->GetImageCount(); i++)
	{
		FrameSync sync;
		sync.commandPool = CommandPool::CreatePool(mDevice, mDevice->GetGraphicsQueue().index, false, true);
		sync.commandBuffer = sync.commandPool->AllocateCommandBuffer();
		sync.imageAvailable = Semaphore::Create(mDevice);
		sync.renderFinished = Semaphore::Create(mDevice);
		sync.inFlightFence = GPUFence::Create(mDevice, true);
		mFrameSyncs.emplace_back(std::move(sync));
	}

	//TEMP
	mVertexShader = VertexShader::Create("Triangle_VS.spv");
	mFragmentShader = FragmentShader::Create("Triangle_FS.spv");

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)mSwapChain->GetExtent().width;
	viewport.height = (float)mSwapChain->GetExtent().height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = mSwapChain->GetExtent();

	std::vector<VkDynamicState> dynamicStates =
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkFormat colorAttachmentFormats[] = { VK_FORMAT_B8G8R8A8_SRGB };

	PipelineBuilder builder;
	mPipeline = builder
		.SetShaders({ mVertexShader->GetStageCreateInfo(), mFragmentShader->GetStageCreateInfo() })
		.SetDefaultBlendStates(1)
		.SetInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
		.SetViewportState({ viewport }, { scissor })
		.SetRasterizer(VK_CULL_MODE_NONE)
		//.SetDynamicStates(dynamicStates)
		.SetDefaultMultisampling()
		.SetRenderingInfo(colorAttachmentFormats, 1, VK_FORMAT_UNDEFINED, VK_FORMAT_UNDEFINED)
		.SetVertexLayout(VertexLayoutType::None)
		.Build();

	mOffscreenQuadTexture = TextureFactory::CreateTextureRenderTarget(mSwapChain->GetExtent(), VK_FORMAT_B8G8R8A8_SRGB, "Offscreen Full Quad");

	return true;
}

void Renderer::BeginFrame()
{
	mDrawCalls.Reset();

	FrameSync& frameSync = mFrameSyncs[mCurrentFrameIndex];

	frameSync.inFlightFence->Wait();
	frameSync.inFlightFence->Reset();

	frameSync.perFrameBuffers.clear();

	VkResult result = vkAcquireNextImageKHR(*mDevice, *mSwapChain, UINT64_MAX, *frameSync.imageAvailable, VK_NULL_HANDLE, &frameSync.imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		mSwapChain->ReCreate(mPhysicalDevice, mDevice, mSurface);
		return;
	}

	frameSync.commandBuffer->Reset();
	frameSync.commandBuffer->Begin();

	GPUTexture* swapChainTexture = mGPUResourceManager.GetResource(mSwapChain->GetRenderTarget(frameSync.imageIndex));
	GPUBarrier::TransitionFromSwapchain(frameSync.commandBuffer->GetRaw(), swapChainTexture);

	//GPUBarrier::TransitionFromSwapchain(frameSync.commandBuffer->GetRaw(), mSwapChain->GetRenderTarget(frameSync.imageIndex)->GetImage(), mSwapChain->GetRenderTarget(frameSync.imageIndex)->GetCurrentImageLayoutRef());

	//GPUBarrier::TransitionToShaderWrite(frameSync.commandBuffer->GetRaw(), mOffscreenQuadTexture->GetImage(), mOffscreenQuadTexture->GetCurrentImageLayoutRef());
	//
	////TODO: Add process of rendergraph
	//
	//{
	//	RenderScope scope(frameSync.commandBuffer, RenderScopeInfo(mOffscreenQuadTexture->GetImageView(), mOffscreenQuadTexture->GetExtent(), Color(1.0f, 0.8f, 0.2f, 1.0f)));
	//	vkCmdBindPipeline(*frameSync.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *mPipeline);
	//	vkCmdDraw(*frameSync.commandBuffer, 3, 1, 0, 0);
	//}
	//GPUBarrier::TransitionToShaderRead(frameSync.commandBuffer->GetRaw(), mOffscreenQuadTexture->GetImage(), mOffscreenQuadTexture->GetCurrentImageLayoutRef());

	////////////////////
	//GPUBarrier::TransitionToSwapchain(frameSync.commandBuffer->GetRaw(), mSwapChain->GetRenderTarget(frameSync.imageIndex)->GetImage(), mSwapChain->GetRenderTarget(frameSync.imageIndex)->GetCurrentImageLayoutRef());

}

void Renderer::EndFrame()
{
	

	FrameSync& frameSync = mFrameSyncs[mCurrentFrameIndex];

	mGPUResourceManager.ExecuteUploads(frameSync.commandBuffer);

	GPUTexture* swapChainTexture = mGPUResourceManager.GetResource(mSwapChain->GetRenderTarget(frameSync.imageIndex));
	GPUBarrier::TransitionToSwapchain(frameSync.commandBuffer->GetRaw(), swapChainTexture);

	frameSync.commandBuffer->End();

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { *frameSync.imageAvailable };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = frameSync.commandBuffer->GetAdressOf();

	VkSemaphore signalSemaphores[] = { *frameSync.renderFinished };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(mDevice->GetGraphicsQueue().queue, 1, &submitInfo, *frameSync.inFlightFence) != VK_SUCCESS)
	{
		_THROW_RENDERER("Failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { *mSwapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &frameSync.imageIndex;
	presentInfo.pResults = nullptr; // Optional

	VkResult result = vkQueuePresentKHR(mDevice->GetPresentQueue().queue, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
	{
		mSwapChain->ReCreate(mPhysicalDevice, mDevice, mSurface);
	}

	mCurrentFrameIndex = (mCurrentFrameIndex + 1) % mFrameSyncs.size();
}

static VKAPI_ATTR VkBool32 VKAPI_CALL ValidationLayerMessageCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData
)
{
	switch (messageSeverity)
	{
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		_LOG_RENDERER_INFO("Validation layer: {}", pCallbackData->pMessage);
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		_LOG_RENDERER_WARNING("Validation layer: {}", pCallbackData->pMessage);
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		_LOG_RENDERER_ERROR("Validation layer: {}", pCallbackData->pMessage);
		break;
	default:
		_THROW_RENDERER("Severity bit has been set on instance creation but no logging has been implemented");
		break;
	}
	return VK_FALSE;
}

bool Renderer::CreateInstance()
{
	_ASSERT_RENDERER(CheckValidationLayerSupport(), "Trying to use validation layer on non supported GPU");

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = Application::Get()->GetApplication()->GetWindowInfo().name;
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0); //TODO: Change to actual version, either from premake or config file
	appInfo.pEngineName = "Unicorn Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0); //TODO: Change to actual version, either from premake or config file
	appInfo.apiVersion = VK_API_VERSION_1_3;

	const char* extensions[] =
	{
		VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef _Win32
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif // _Win32
		VK_EXT_DEBUG_UTILS_EXTENSION_NAME
	};

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	debugCreateInfo.messageSeverity =
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

	if (VK_ENABLE_VALIDATION_LAYER_INFO_LOGGING)
		debugCreateInfo.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;

	debugCreateInfo.messageType =
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	debugCreateInfo.pfnUserCallback = ValidationLayerMessageCallback;
	debugCreateInfo.pUserData = nullptr;

	VkInstanceCreateInfo createInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = _countof(extensions);
	createInfo.ppEnabledExtensionNames = extensions;

	createInfo.enabledLayerCount = static_cast<uint32>(GetValidationLayers().size());
	createInfo.ppEnabledLayerNames = GetValidationLayers().data();

	createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;

	VkResult result = vkCreateInstance(&createInfo, nullptr, &mInstance);
	if (result != VK_SUCCESS)
	{
		_ENSURE_RENDERER(false, "Failed to create Vulkan instance: {}", string_VkResult(result));
		return false;
	}

	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
		mInstance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		func(mInstance, &debugCreateInfo, nullptr, &mDebugMessenger);
	}

	return true;
}

bool Renderer::CheckValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : GetValidationLayers())
	{
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}

		if (!layerFound)
		{
			return false;
		}
	}

	return true;
}

const std::vector<const char*>& Renderer::GetValidationLayers()
{
	static const std::vector<const char*> validationLayers =
	{
		"VK_LAYER_KHRONOS_validation"
	};

	return validationLayers;
}
