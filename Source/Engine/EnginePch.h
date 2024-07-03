#pragma once
#pragma message("Pch Engine!")

#include <SimpleMath.h>
#include <UnicornMath.h>

#include <d3d11.h>
#include <vector>
#include <array>
#include <bitset>
#include <memory>
#include <typeindex>
#include <unordered_set>
#include <unordered_map>
#include <tuple>
#include <assert.h>
#include <functional>
#include <any>

#include "source/Engine.h"
#include "source/reflection/Reflection.h"
#include "source/Global.h"
#include "source/ecs/ECSWorld.h"
#include "source/ecs/SystemManager.h"
#include <source/GraphicsEngine.h>

#include "ecs/components/Transform.h"
#include "ecs/components/Mesh.h"
#include "ecs/components/Name.h"
#include "ecs/components/Tag.h"
#include "ecs/components/ChildParent.h"