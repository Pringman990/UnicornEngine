#include "EditorPch.h"
#include "ImguiImpl.h"

#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_internal.h>

#include <Window.h>
#include <DX11.h>

ImguiImpl::ImguiImpl()
{
}

ImguiImpl::~ImguiImpl()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

bool ImguiImpl::Init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;
	ImGuiIO& io = ImGui::GetIO();

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
		io.Fonts->AddFontFromFileTTF("..\\EngineAssets\\Fonts\\SalmaproMedium-0Wooo.ttf", 14.f, &config, ranges);

		io.Fonts->Build();
	}

	// merge in icons from Font Awesome
	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;

	io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAS, 11.0f, &icons_config, icons_ranges);
	io.Fonts->Build();

	if (!ImGui_ImplWin32_Init(Engine::GetGraphicsEngine().GetCurrentWindow().GetWindowInfo().currentWindow))
		return false;
	if (!ImGui_ImplDX11_Init(Engine::GetGraphicsEngine().GetDX11()->GetDevice(), Engine::GetGraphicsEngine().GetDX11()->GetDeviceContext()))
		return false;

	return true;
}

void ImguiImpl::BeginFrame()
{
	ImGui_ImplWin32_NewFrame();
	ImGui_ImplDX11_NewFrame();
	ImGui::NewFrame();
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
}

void ImguiImpl::EndFrame()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
}