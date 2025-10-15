#include "Win32DX11ImguiBackend.h"
#include <Application/Windows/WindowsApplication.h>

#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>
#include "IconFontDefines.h"

#include <Renderer.h>

Win32DX11ImguiBackend::Win32DX11ImguiBackend()
{
}

Win32DX11ImguiBackend::~Win32DX11ImguiBackend()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

bool Win32DX11ImguiBackend::Init()
{
	_PAUSE_TRACK_MEMORY(true);
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;
	ImGuiIO& io = ImGui::GetIO();

	ImGui::LoadIniSettingsFromDisk("imgui.ini");

	std::string contentPath = SubsystemManager::Get<FileSystem>()->GetAbsolutPath("engine://");
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
		io.Fonts->AddFontFromFileTTF((contentPath + "SalmaproMedium-0Wooo.ttf").c_str(), 14.f, &config, ranges);

		//io.Fonts->Build();
	}

	// merge in icons from Font Awesome
	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;

	io.Fonts->AddFontFromFileTTF((contentPath + "fa-solid-900.ttf").c_str(), 11.0f, &icons_config, icons_ranges);
	//io.Fonts->Build();

	_PAUSE_TRACK_MEMORY(false);

	Application* app = SubsystemManager::Get<Application>();

	WindowsApplication* windowsApp = static_cast<WindowsApplication*>(app->GetApplication());
	windowsApp->OnWndProc.AddRaw(this, &Win32DX11ImguiBackend::ProccessMessages);

	if (!ImGui_ImplWin32_Init(windowsApp->GetWindowsWindowInfo().windowHandle.hwnd))
		return false;

	Renderer* renderer = SubsystemManager::Get<Renderer>();

	if (!ImGui_ImplDX11_Init(renderer->GetLogicalDevice().GetRaw(), renderer->GetLogicalDevice().GetImmediateContext()))
		return false;

	return true;
}

void Win32DX11ImguiBackend::BeginFrame()
{
	ImGui_ImplWin32_NewFrame();
	ImGui_ImplDX11_NewFrame();
	ImGui::NewFrame();
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
}

void Win32DX11ImguiBackend::RenderFrame()
{
	ImGui::Render();
}

void Win32DX11ImguiBackend::EndFrame()
{
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool Win32DX11ImguiBackend::ProccessMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	return false;
}

void Win32DX11ImguiBackend::ResizeBackBuffer(Vector2 NewSize)
{
}
