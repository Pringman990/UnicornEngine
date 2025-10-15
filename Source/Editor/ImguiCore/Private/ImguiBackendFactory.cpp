#include "pch.h"
#include "ImguiBackendFactory.h"

#ifdef _Win32
#include "Win32/Win32DX11ImguiBackend.h"
#include "Win32/Win32DX12ImguiBackend.h"
#include "Win32/Win32VulkanImguiBackend.h"
#endif

IImguiBackend* ImguiBackendFactory::CreateBackend()
{
	//TODO: Change to actually selected Graphics backend from engine config file
#ifdef _Win32
	switch (2)
	{
	case 0:
		return new Win32DX12ImguiBackend();
	case 1:
		return new Win32VulkanImguiBackend();
	case 2:
		return new Win32DX11ImguiBackend();
	default:
		break;
	}
#endif // _Win32


    return nullptr;
}
