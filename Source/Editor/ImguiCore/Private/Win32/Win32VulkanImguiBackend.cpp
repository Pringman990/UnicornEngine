#include "pch.h"
#include "Win32VulkanImguiBackend.h"

#include <Application/Windows/WindowsApplication.h>

#include <Renderer.h>

Win32VulkanImguiBackend::Win32VulkanImguiBackend()
{
}

Win32VulkanImguiBackend::~Win32VulkanImguiBackend()
{
}

bool Win32VulkanImguiBackend::Init()
{
	_PAUSE_TRACK_MEMORY(true);
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;
	
	ImGui::GetPlatformIO().Platform_CreateVkSurface = [](ImGuiViewport* viewport, ImU64 vk_instance, const void* vk_allocator, ImU64* out_surface) -> int
		{
			VkInstance instance = (VkInstance)vk_instance;

			Surface* surface = Surface::Create(viewport->PlatformHandle, instance);
			if (!surface)
				return VK_ERROR_UNKNOWN;

			*out_surface = (ImU64)surface->GetRaw();

			return 0;
		};

	//ImGui::LoadIniSettingsFromDisk("imgui.ini");

	//{
		//static const ImWchar ranges[] =
		//{
		//	0x0020, 0x00FF, // Basic Latin + Latin Supplement
		//	0x0104, 0x017C, // Polish characters and more
		//	0,
		//};

	//ImFontConfig config;
	//config.OversampleH = 4;
	//config.OversampleV = 4;
	//config.PixelSnapH = false;

	//TODO: Load from memory using loader and not path
	//io.Fonts->AddFontFromFileTTF(FileSystem::GetAbsolutPath("engine://Content/SalmaproMedium-0Wooo.ttf").c_str(), 14.f, &config, ranges);

	//io.Fonts->Build();
	//}

	//// merge in icons from Font Awesome
	//static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
	//ImFontConfig icons_config;
	//icons_config.MergeMode = true;
	//icons_config.PixelSnapH = true;

	////TODO: Load from memory using loader and not path
	////io.Fonts->AddFontFromFileTTF((contentPath + "\\fa-solid-900.ttf").c_str(), 11.0f, &icons_config, icons_ranges);
	////io.Fonts->Build();

	_PAUSE_TRACK_MEMORY(false);

	WindowsApplication* windowsApp = static_cast<WindowsApplication*>(Application::Get()->GetApplication());
	if (!ImGui_ImplWin32_Init(windowsApp->GetWindowsWindowInfo().windowHandle))
		return false;

	windowsApp->OnWndProc.AddRaw(this, &Win32VulkanImguiBackend::ProccessMessages);

	Renderer* renderer = Renderer::Get();

	VkPipelineRenderingCreateInfoKHR prci{};
	prci.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
	prci.colorAttachmentCount = 1;
	prci.pColorAttachmentFormats = &renderer->GetSwapChain()->GetImageFormat();
	prci.depthAttachmentFormat = VK_FORMAT_UNDEFINED;
	prci.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;
	prci.viewMask = 0;

	//TODO: Add info
	ImGui_ImplVulkan_InitInfo initInfo{};
	initInfo.ApiVersion = VK_API_VERSION_1_3;
	initInfo.Instance = renderer->GetInstance();
	initInfo.PhysicalDevice = *renderer->GetPhysicalDevice();
	initInfo.Device = *renderer->GetDevice();
	initInfo.Queue = renderer->GetDevice()->GetGraphicsQueue().queue;
	initInfo.QueueFamily = renderer->GetDevice()->GetGraphicsQueue().index;
	initInfo.DescriptorPool = VK_NULL_HANDLE;
	initInfo.DescriptorPoolSize = 1000; //Change to internal descriptor pool later
	initInfo.ImageCount = renderer->GetSwapChain()->GetImageCount();
	initInfo.MinImageCount = renderer->GetSwapChain()->GetMinSupportedImageCount();
	//initInfo.MinAllocationSize = 1024 * 1024; //This can be changed with custom vulkan memory allocator
	//initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	initInfo.PipelineRenderingCreateInfo = prci;
	initInfo.UseDynamicRendering = true;

	if (!ImGui_ImplVulkan_Init(&initInfo))
		return false;

	//renderer->OnBackbufferResize.AddRaw(this, &Win32DX12ImguiBackend::ResizeBackBuffer);
	return true;
}

void Win32VulkanImguiBackend::BeginFrame()
{
	ImGui_ImplWin32_NewFrame();
	ImGui_ImplVulkan_NewFrame();
	ImGui::NewFrame();
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
}

void Win32VulkanImguiBackend::RenderFrame()
{
	ImGui::ShowDemoWindow();
}

void Win32VulkanImguiBackend::EndFrame(CommandBuffer* Buffer)
{
	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *Buffer);
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
}

//void Win32VulkanImguiBackend::AddTextureToImgui(Texture2D* Texture, Sampler* Sampler)
//{
//	GPUTexture* gpuTexture = Renderer::Get()->GetGPUResourceManager().GetResource(Texture->GetResourceData().gpuHandle);
//	uint64 handle = (ImTextureID)ImGui_ImplVulkan_AddTexture(*Sampler, gpuTexture->imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
//
//	Texture->SetImGuiHandle(handle);
//}
//
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool Win32VulkanImguiBackend::ProccessMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	return false;
}

void Win32VulkanImguiBackend::ResizeBackBuffer(Vector2 NewSize)
{
	//ImGui_ImplVulkanH_CreateOrResizeWindow();
}
