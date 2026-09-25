//
// Created on 2026-07-14.
//

#include "../../pch.h"
#include "UnicornAssert.h"

#include "Logs.h"

namespace Assert
{
    void DebugBreak()
    {
#if defined(_MSC_VER)
        __debugbreak();
#elif defined(__clang__) || defined(__GNUC__)
        __builtin_trap();
#endif
    }
}
