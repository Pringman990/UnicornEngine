#pragma once
#pragma message("Pch Editor!")
#pragma comment(lib, "d3d11.lib")

#include <d3d11.h>

#include <SimpleMath.h>
#include <UnicornMath.h>

#include <comdef.h>
#include <bitset>
#include <memory>
#include <string>
#include <array>
#include <vector>
#include <unordered_map>
#include <map>
#include <any>
#include <typeindex>

#include <source/internals/Editor.h>
#include <source/Engine.h>
#include <source/Global.h>
#include <source/utility/StringConvert.h>
#include <source/GraphicsEngine.h>
#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include <source/input/InputManager.h>
#include <source/reflection/Reflection.h>
#include <source/ecs/ECSWorld.h>
#include <source/internals/utility/Utility.h>
