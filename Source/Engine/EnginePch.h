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
#include "source/Global.h"
#include "source/ecs/ECSWorld.h"
#include <source/GraphicsEngine.h>

#define ASSERT_MSG(cond, msg) do { \
    if (!(cond)) { \
        std::cerr << "Assertion failed: " << (msg) << "\n"; \
        assert(cond); \
    } \
} while (0)