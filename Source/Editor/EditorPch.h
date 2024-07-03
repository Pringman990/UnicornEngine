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

#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>

#include <Engine.h>
#include <ecs/ECSWorld.h>
#include <ecs/SystemManager.h>
#include <input/InputManager.h>
#include <reflection/Reflection.h>
#include <utility/StringConvert.h>
#include <ecs/components/EngineComponents.h>

#include <GraphicsEngine.h>

#include <source/internals/EditorCore.h> 
#include <source/internals/utility/Utility.h>
#include "internals/utility/IconFontDefines.h"