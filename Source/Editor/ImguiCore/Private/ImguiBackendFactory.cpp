#include "pch.h"
#include "ImguiBackendFactory.h"

#ifdef _Win32
#include "Win32/Win32DX11ImguiBackend.h"
#endif

IImguiBackend* ImguiBackendFactory::CreateBackend()
{
#ifdef _Win32
	switch (0)
	{
	case 0:
		return new Win32DX11ImguiBackend();
	default:
		break;
	}
#endif // _Win32


    return nullptr;
}