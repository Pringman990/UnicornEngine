#include "Renderer.h"

#ifdef _Win32
#include <Application/Windows/WindowsApplication.h>
#endif // _Win32

#include <FileSystem/NativeFileBackend.h>

#include "ResourceLoaders/Texture2DLoader.h"
#include "Factories/TextureFactory.h"
#include "Factories/MeshFactory.h"

#include "RenderScope.h"
#include "GPUBarrier.h"

#include "Pipeline.h"
#include "PipelineBuilder.h"
#include "VertexShader.h"
#include "FragmentShader.h"
#include "ComputeShader.h"
#include "GPUTexture.h"
#include "GPUMesh.h"

#include "DescriptorLayoutBuilder.h"
#include "DescriptorPoolBuilder.h"
#include "DescriptorWriter.h"

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
	vkDeviceWaitIdle(*mDevice);

	for (uint32 i = 0; i < mFrameSyncs.size(); i++)
	{
		mFrameSyncs[i].Destroy();
	}
	mFrameSyncs.clear();

	delete mSwapChain;
	mSwapChain = nullptr;

	UnRegisterAllGPUResourcePools();

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
	RegisterAllGPUResourcePools();

	if (!CreateInstance())
		return false;

	mSurface = Surface::Create(mInstance);
	mPhysicalDevice = PhysicalDevice::Create(mInstance, mSurface);
	mDevice = mPhysicalDevice->CreateLogicalDevice();
	mSwapChain = SwapChain::Create(mPhysicalDevice, mDevice, mSurface);
	uint32 swapChainImageCount = mSwapChain->GetImageCount();

	mSetLayout =
		DescriptorLayoutBuilder()
		.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
		.AddBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
		.Build(mDevice);

	mDescPool =
		DescriptorPoolBuilder()
		.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2)
		.Build(mDevice, swapChainImageCount);

	mFrameSyncs.reserve(swapChainImageCount);
	for (uint32 i = 0; i < mSwapChain->GetImageCount(); i++)
	{
		FrameSync sync;
		sync.commandPool = CommandPool::CreatePool(mDevice, mDevice->GetGraphicsQueue().index, false, true);
		sync.commandBuffer = sync.commandPool->AllocateCommandBuffer();
		sync.imageAvailable = Semaphore::Create(mDevice);
		sync.renderFinished = Semaphore::Create(mDevice);
		sync.inFlightFence = GPUFence::Create(mDevice, true);

		sync.cameraUBOBuffer = GenericGPUBuffer::Create(sizeof(CameraUBO), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		sync.objectFrameDataUBOBuffer = GenericGPUBuffer::Create(sizeof(ObjectFrameDataUBO), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		sync.descSet =
			DescriptorWriter(mSetLayout, mDescPool)
			.WriteBuffer(0, { .buffer = sync.cameraUBOBuffer->GetRaw(), .offset = 0, .range = sizeof(CameraUBO)})
			.WriteBuffer(1, { .buffer = sync.objectFrameDataUBOBuffer->GetRaw(), .offset = 0, .range = sizeof(ObjectFrameDataUBO)})
			.Build(mDevice);

		mFrameSyncs.emplace_back(std::move(sync));
	}


	ByteBuffer meshData = FileSystem::Get()->ReadAll("engine://Models/sm_cube.fbx");
	GPUResourceHandle<GPUMesh> meshHandle = MeshFactory::CreateMesh(meshData, "Cube");
	mMesh = mGPUResourceManager.GetResource(meshHandle);

	mVertexShader = VertexShader::Create("Mesh_VS.spv");
	mFragmentShader = FragmentShader::Create("Single_Color_Mesh_FS.spv");

	Vector<VkPipelineShaderStageCreateInfo> shaders{ mVertexShader->GetStageCreateInfo(), mFragmentShader->GetStageCreateInfo() };

	viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)mSwapChain->GetExtent().width;
	viewport.height = (float)mSwapChain->GetExtent().height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = mSwapChain->GetExtent();

	std::vector<VkDynamicState> dynamicStates =
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkFormat colorAttachmentFormats[] = { VK_FORMAT_B8G8R8A8_SRGB };

	mPipeline =
		PipelineBuilder()
		.SetShaders(shaders)
		.SetInputAssembly(VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
		.SetDefaultMultisampling()
		.SetDefaultBlendStates(1)
		.SetViewportState({ viewport }, { scissor })
		.SetRenderingInfo(colorAttachmentFormats, 1, VK_FORMAT_UNDEFINED, VK_FORMAT_UNDEFINED)
		.SetVertexLayout(VertexLayoutType::Vertex3D)
		.SetRasterizer(VK_CULL_MODE_NONE)
		.SetDescriptorSetLayouts({ mSetLayout })
		//.SetDynamicStates(dynamicStates)
		.Build();


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

	CameraUBO cameraUBO{};
	cameraUBO.viewMatrix = mActiveCamera->GetViewMatrix();
	cameraUBO.projMatrix = mActiveCamera->GetProjectionMatrix();
	cameraUBO.cameraPosition = mActiveCamera->GetPosition();
	frameSync.cameraUBOBuffer->Map(&cameraUBO, sizeof(CameraUBO));

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

void Renderer::RenderMesh(const GPUResourceHandle<GPUMesh>& MeshHandle, Transform& MeshTransform)
{
	FrameSync& frameSync = mFrameSyncs[mCurrentFrameIndex];
	{
		RenderScope scope(frameSync.commandBuffer, RenderScopeInfo(mSwapChain->GetRenderTarget(frameSync.imageIndex)));

		ObjectFrameDataUBO objectUBO{};
		objectUBO.model = MeshTransform.GetMatrix();
		frameSync.objectFrameDataUBOBuffer->Map(&objectUBO, sizeof(ObjectFrameDataUBO));

		vkCmdBindDescriptorSets(*frameSync.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline->GetLayout(), 0, 1, frameSync.descSet->GetAdressOf(), 0, VK_NULL_HANDLE);

		vkCmdBindPipeline(*frameSync.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *mPipeline);

		GPUMesh* mesh = mGPUResourceManager.GetResource(MeshHandle);
		vkCmdBindIndexBuffer(*frameSync.commandBuffer, mesh->indexBuffer, 0, VK_INDEX_TYPE_UINT32);

		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(*frameSync.commandBuffer, 0, 1, mesh->vertexBuffer.GetAdressOf(), offsets);


		for (auto& submesh : mesh->subMeshes)
		{
			vkCmdDrawIndexed(*frameSync.commandBuffer, submesh.indexCount, 1, submesh.indexOffset, 0, 0);
		}
	}
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

void Renderer::RegisterAllGPUResourcePools()
{
	mGPUResourceManager.RegisterPool<GPUTexture>();
	mGPUResourceManager.RegisterPool<GPUMesh>();
}

void Renderer::UnRegisterAllGPUResourcePools()
{
	mGPUResourceManager.UnRegisterPool<GPUMesh>(&GPUMesh::Destroy, mDevice);
	mGPUResourceManager.UnRegisterPool<GPUTexture>(&GPUTexture::DestroyGPUTexture, mDevice);
}

const std::vector<const char*>& Renderer::GetValidationLayers()
{
	static const std::vector<const char*> validationLayers =
	{
		"VK_LAYER_KHRONOS_validation"
	};

	return validationLayers;
}
