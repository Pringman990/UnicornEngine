#pragma once
#pragma message("pch Graphics Engine")
#pragma comment(lib, "d3d11.lib")

#include <d3d11.h>
#include <d3d11_1.h>

#include <SimpleMath.h>
#include <UnicornMath.h>
#include <comdef.h>
#include <bitset>
#include <array>
#include <vector>
#include <unordered_map>
#include <map>

#include <source/Engine.h>
#include <source/utility/StringConvert.h>

#include "source/GraphicsEngine.h"
#include "source/Vertex.h"

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#define NOMINMAX // Don’t get strange min and max defines
#define OUT