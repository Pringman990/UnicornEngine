#pragma once
#pragma message("Pch Engine!")

#include <SimpleMath.h>
#include <UnicornMath.h>

#include <bitset>
#include <memory>
#include <typeindex>
#include <unordered_set>
#include <tuple>
#include <assert.h>

#include "source/Engine.h"
#include <source/GraphicsEngine.h>

#define ASSERT_MSG(cond, msg) do { \
    if (!(cond)) { \
        std::cerr << "Assertion failed: " << (msg) << "\n"; \
        assert(cond); \
    } \
} while (0)