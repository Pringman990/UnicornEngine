#pragma once
#pragma message("pch DX11Renderer!")

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <d3d11.h>
#include <d3d11_1.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <comdef.h>

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#include <vector>
#include <memory>
#include <string>

#include <Vertex.h>

#include <ResourceRegistry.h>
#include <RenderTarget/RenderTargetResourceManager.h>

#include <StandardTypes.h>
#include <Logger.h>
#include <Utility.h>

#include <SimpleMath.h>

#include "DX11RenderingBackend.h"