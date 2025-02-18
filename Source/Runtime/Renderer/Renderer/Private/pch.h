#pragma once
#pragma message("pch Renderer!")

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
#include <fstream>

#include <Vertex.h>
#include <Shader.h>

#include <StandardTypes/StandardTypes.h>
#include <Logger/Logger.h>
#include <Utility/Utility.h>

#include <SimpleMath.h>

#include <Singleton.h>
#include "Renderer.h"