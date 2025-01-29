#include "pch.h"
#include "Win32DX11ImguiBackend.h"

#include <Application/Windows/WindowsApplication.h>
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
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;
	ImGuiIO& io = ImGui::GetIO();

	ImGui::LoadIniSettingsFromDisk("imgui.ini");

	std::string contentPath = FileWatcher::GetInstance()->GetContentPath();
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
		io.Fonts->AddFontFromFileTTF((contentPath + "\\SalmaproMedium-0Wooo.ttf").c_str(), 14.f, &config, ranges);

		io.Fonts->Build();
	}

	// merge in icons from Font Awesome
	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;

	io.Fonts->AddFontFromFileTTF((contentPath + "\\fa-solid-900.ttf").c_str(), 11.0f, &icons_config, icons_ranges);
	io.Fonts->Build();

	WindowsApplication* windowsApp = static_cast<WindowsApplication*>(Application::GetInstance()->GetApplication());
	if (!ImGui_ImplWin32_Init(windowsApp->GetWindowsWindowInfo().windowHandle))
		return false;

	windowsApp->AddWinProcObserver(this);

	Renderer* renderer = Renderer::GetInstance();
	_ENSURE_EDITOR(renderer, "We are using win32 dx11 but renderer is not dx11?");

	if (!ImGui_ImplDX11_Init(renderer->GetDevice(), renderer->GetDeviceContext()))
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

void Win32DX11ImguiBackend::EndFrame()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void Win32DX11ImguiBackend::ProccessMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam);
}
