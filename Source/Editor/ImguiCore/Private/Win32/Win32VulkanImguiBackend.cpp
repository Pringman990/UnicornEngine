#include "pch.h"
#include "Win32VulkanImguiBackend.h"

#include <Application/Windows/WindowsApplication.h>

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
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;
	ImGuiIO& io = ImGui::GetIO();
	
	ImGui::LoadIniSettingsFromDisk("imgui.ini");

	{
		static const ImWchar ranges[] =
		{
			0x0020, 0x00FF, // Basic Latin + Latin Supplement
			0x0104, 0x017C, // Polish characters and more
			0,
		};

		ImFontConfig config;
		config.OversampleH = 4;
		config.OversampleV = 4;
		config.PixelSnapH = false;

		//TODO: Load from memory using loader and not path
		//io.Fonts->AddFontFromFileTTF((contentPath + "\\SalmaproMedium-0Wooo.ttf").c_str(), 14.f, &config, ranges);

		io.Fonts->Build();
	}

	// merge in icons from Font Awesome
	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;

	//TODO: Load from memory using loader and not path
	//io.Fonts->AddFontFromFileTTF((contentPath + "\\fa-solid-900.ttf").c_str(), 11.0f, &icons_config, icons_ranges);
	io.Fonts->Build();

	_PAUSE_TRACK_MEMORY(false);

	WindowsApplication* windowsApp = static_cast<WindowsApplication*>(Application::Get()->GetApplication());
	if (!ImGui_ImplWin32_Init(windowsApp->GetWindowsWindowInfo().windowHandle))
		return false;

	//TODO: Add info
	ImGui_ImplVulkan_InitInfo initInfo{};
	
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
	ImGui::Render();
}

void Win32VulkanImguiBackend::EndFrame()
{
	//ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), Renderer::Get()->GetMainCommandList());
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
}

void Win32VulkanImguiBackend::ResizeBackBuffer(Vector2 NewSize)
{
	//ImGui_ImplVulkanH_CreateOrResizeWindow();
}
