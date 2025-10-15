#pragma once

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#define NOMINMAX // Don’t get strange min and max defines
#include <windows.h>

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#include "RenderFormat.h"