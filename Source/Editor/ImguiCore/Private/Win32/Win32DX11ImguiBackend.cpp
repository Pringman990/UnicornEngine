#include "pch.h"
#include "Win32DX11ImguiBackend.h"

#include <Windows/WindowsApplication.h>
#include <DX11RenderingBackend.h>

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
		io.Fonts->AddFontFromFileTTF("..\\..\\Assets\\SalmaproMedium-0Wooo.ttf", 14.f, &config, ranges);

		io.Fonts->Build();
	}

	// merge in icons from Font Awesome
	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;

	io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAS, 11.0f, &icons_config, icons_ranges);
	io.Fonts->Build();

	WindowsApplication* windowsApp = static_cast<WindowsApplication*>(Application::GetInstance()->GetApplication());
	if (!ImGui_ImplWin32_Init(windowsApp->GetWindowsWindowInfo().windowHandle))
		return false;

	windowsApp->AddWinProcObserver(this);

	DX11RenderingBackend* dx11Renderer = static_cast<DX11RenderingBackend*>(Renderer::GetInstance()->GetRenderingBackend());
	_ENSURE_EDITOR(dx11Renderer, "We are using win32 dx11 but renderer is not dx11?");

	if (!ImGui_ImplDX11_Init(dx11Renderer->GetDevice(), dx11Renderer->GetDeviceContext()))
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
