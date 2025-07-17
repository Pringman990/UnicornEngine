#include "pch.h"
#include "Win32DX12ImguiBackend.h"

#include <Application/Windows/WindowsApplication.h>
#include <Renderer.h>

Win32DX12ImguiBackend::Win32DX12ImguiBackend()
{
}

Win32DX12ImguiBackend::~Win32DX12ImguiBackend()
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

bool Win32DX12ImguiBackend::Init()
{
	//_PAUSE_TRACK_MEMORY(true);
	//IMGUI_CHECKVERSION();
	//ImGui::CreateContext();
	//ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;
	//ImGuiIO& io = ImGui::GetIO();
	//
	//ImGui::LoadIniSettingsFromDisk("imgui.ini");

	//std::string contentPath = WindowsFileWatcher::Get()->GetContentPath();
	//{
	//	static const ImWchar ranges[] =
	//	{
	//		0x0020, 0x00FF, // Basic Latin + Latin Supplement
	//		0x0104, 0x017C, // Polish characters and more
	//		0,
	//	};

	//	ImFontConfig config;
	//	config.OversampleH = 4;
	//	config.OversampleV = 4;
	//	config.PixelSnapH = false;
	//	io.Fonts->AddFontFromFileTTF((contentPath + "\\SalmaproMedium-0Wooo.ttf").c_str(), 14.f, &config, ranges);

	//	io.Fonts->Build();
	//}

	//// merge in icons from Font Awesome
	//static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
	//ImFontConfig icons_config;
	//icons_config.MergeMode = true;
	//icons_config.PixelSnapH = true;

	//io.Fonts->AddFontFromFileTTF((contentPath + "\\fa-solid-900.ttf").c_str(), 11.0f, &icons_config, icons_ranges);
	//io.Fonts->Build();

	//_PAUSE_TRACK_MEMORY(false);

	//WindowsApplication* windowsApp = static_cast<WindowsApplication*>(Application::Get()->GetApplication());
	//if (!ImGui_ImplWin32_Init(windowsApp->GetWindowsWindowInfo().windowHandle))
	//	return false;

	////TODO:fix this
	////windowsApp->OnWndProc.AddRaw(this, &Win32DX12ImguiBackend::ProccessMessages);

	//Renderer* renderer = Renderer::Get();
	//ID3D12DescriptorHeap* srvHeap = renderer->GetSRVHeapManager().GetHeap();

	//if (!ImGui_ImplDX12_Init(
	//	renderer->GetDevice(), 
	//	BACKBUFFER_COUNT, 
	//	DXGI_FORMAT_R8G8B8A8_UNORM,
	//	srvHeap,
	//	srvHeap->GetCPUDescriptorHandleForHeapStart(),
	//	srvHeap->GetGPUDescriptorHandleForHeapStart()
	//))
	//	return false;
	//
	//renderer->OnBackbufferResize.AddRaw(this, &Win32DX12ImguiBackend::ResizeBackBuffer);
	return true;
}

void Win32DX12ImguiBackend::BeginFrame()
{
	ImGui_ImplWin32_NewFrame();
	ImGui_ImplDX12_NewFrame();
	ImGui::NewFrame();
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode); 
}

void Win32DX12ImguiBackend::RenderFrame()
{
	ImGui::Render();
}

void Win32DX12ImguiBackend::EndFrame(CommandBuffer* Buffer)
{
	/*ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), Renderer::Get()->GetMainCommandList());
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();*/
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void Win32DX12ImguiBackend::ProccessMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam);
}

void Win32DX12ImguiBackend::AddTextureToImgui(Texture2D* Texture, Sampler* Sampler)
{
}

void Win32DX12ImguiBackend::ResizeBackBuffer(Vector2 aNewSize)
{
	ImGui_ImplDX12_InvalidateDeviceObjects();
	ImGui_ImplDX12_CreateDeviceObjects();
}
