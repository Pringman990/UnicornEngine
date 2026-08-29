//
// Created on 2026-07-14.
//

#include "../pch.h"
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

    void HandleAssert(StringView expression, std::source_location location)
    {
        Services::Get<Logger>().Log(LogLevel::Fatal, location, "Assertion failed: {}", expression);

        DebugBreak();
    }

    bool HandleVerify(StringView expression, std::source_location location)
    {
        Services::Get<Logger>().Log(LogLevel::Warning, location, "Verification failed: {}", expression);

#ifdef _DEBUG
        DebugBreak();
#endif

        return false;
    }
}
