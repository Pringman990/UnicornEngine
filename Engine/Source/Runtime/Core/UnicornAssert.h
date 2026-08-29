#pragma once
#include <source_location>

#include "Logger.h"
#include "ServiceRegistry.h"
#include "Types.h"
#include "fmt/args.h"

namespace Assert
{
    void DebugBreak();

    /**
     * Ex:
     * ASSERT(renderer != nullptr);

        ASSERT FAILED
        Expression: renderer != nullptr

        Renderer.cpp:42
        RenderFrame()

     */
    void HandleAssert(StringView expression, std::source_location location);

    /**
    * Ex:
    *   if (!VERIFY(window.Init()))
        {
            return false;
        }

        VERIFY FAILED
        Expression: window.Init()

        Window.cpp:55

    */
    bool HandleVerify(StringView expression, std::source_location location);

    /**
    * Ex:
    *   FATAL("Could not initialize graphics device");

        FATAL ERROR
        Could not initialize graphics device

        Renderer.cpp:91

    */
    template <typename... Args>
    NORETURN void HandleFatal(std::source_location location, fmt::format_string<Args...> format, Args&&... args)
    {
        Services::Get<Logger>().Log(LogLevel::Fatal, location, format, args...);

        DebugBreak();

        std::abort();
    }
}

#ifdef _DEBUG

#define ASSERT(expr, info) \
    ((expr) ? void(0) : Assert::HandleAssert(#expr, std::source_location::current()))
#else

#define ASSERT(expr, info) ((void)0)

#endif

#define VERIFY(expr) \
    ((expr) ? true : Assert::HandleVerify(#expr, std::source_location::current()))

#define FATAL(fmt, ...) \
    Assert::HandleFatal(std::source_location::current(), fmt __VA_OPT__(,) __VA_ARGS__)