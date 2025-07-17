#include "pch.h"
#include "Surface.h"

#ifdef _Win32
#include <Application/Windows/WindowsApplication.h>
#endif // _Win32

Surface::Surface()
{
}

Surface::~Surface()
{
	vkDestroySurfaceKHR(Renderer::Get()->GetInstance(), mSurface, nullptr);
	mSurface = VK_NULL_HANDLE;
}

Surface* Surface::Create(VkInstance Instance)
{
	void* hwnd = nullptr;
#ifdef _Win32
	WindowsApplication* windowsApp = static_cast<WindowsApplication*>(Application::Get()->GetApplication());
	auto& windowsInfo = windowsApp->GetWindowsWindowInfo();
	hwnd = windowsInfo.windowHandle;
#else
	assert(false);
#endif // _Win32

	return Create(hwnd, Instance);
}

Surface* Surface::Create(void* Hwnd, VkInstance Instance)
{
	if (!Hwnd)
	{
		_THROW_RENDERER("Failed to create Vulkan surface because hwnd was nullptr");
		return nullptr;
	}

	Surface* surface = new Surface();
	VkResult result = VK_SUCCESS;

#ifdef _Win32
	WindowsApplication* windowsApp = static_cast<WindowsApplication*>(Application::Get()->GetApplication());
	auto& windowsInfo = windowsApp->GetWindowsWindowInfo();

	VkWin32SurfaceCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hwnd = static_cast<HWND>(Hwnd);
	createInfo.hinstance = windowsInfo.hInstance;

	result = vkCreateWin32SurfaceKHR(Instance, &createInfo, nullptr, &surface->mSurface);
#else
	assert(false);
#endif // _Win32

	if (result != VK_SUCCESS)
	{
		_ASSERT_RENDERER(false, "Failed to create Vulkan surface: {}", string_VkResult(result));
		return nullptr;
	}
	return surface;
}