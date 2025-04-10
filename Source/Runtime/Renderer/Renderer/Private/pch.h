#pragma once
#pragma message("pch Renderer!")

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <d3d12.h>
#include <d3dcompiler.h>
#include <comdef.h>

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#include <vector>
#include <memory>
#include <string>
#include <fstream>

#include <Vertex.h>

#include <Core.h>

#include <Singleton.h>
#include "Renderer.h"