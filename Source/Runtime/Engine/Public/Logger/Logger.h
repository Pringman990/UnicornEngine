#pragma once
#include "EngineDefines.h"
#include <MemoryDebugger/MemoryDebugger.h>
#include <StandardTypes/StandardTypes.h>

#include <spdlog/spdlog.h>

namespace Logger
{
    ENGINE_API SharedPtr<spdlog::logger>& GetMain();
    ENGINE_API void Init();
    ENGINE_API void Cleanup();
}

#ifdef _DEBUG

#define _LOGGER_STOP_MEMORY_TRACKING(LOGGER) \
    _PAUSE_TRACK_MEMORY(true);\
    LOGGER; \
    _PAUSE_TRACK_MEMORY(false)

//User Logs
#define LOG_INFO(...)       _LOGGER_STOP_MEMORY_TRACKING(SPDLOG_LOGGER_INFO(Logger::GetMain(), __VA_ARGS__))
#define LOG_WARNING(...)    _LOGGER_STOP_MEMORY_TRACKING(SPDLOG_LOGGER_WARN(Logger::GetMain(), __VA_ARGS__))
#define LOG_ERROR(...)      _LOGGER_STOP_MEMORY_TRACKING(SPDLOG_LOGGER_ERROR(Logger::GetMain(), __VA_ARGS__))
#define LOG_CRITICAL(...)   _LOGGER_STOP_MEMORY_TRACKING(SPDLOG_LOGGER_CRITICAL(Logger::GetMain(), __VA_ARGS__))

#else

#define LOG_INFO(...) (void)(0)
#define LOG_WARNING(...) (void)(0)
#define LOG_ERROR(...) (void)(0)
#define LOG_CRITICAL(...) (void)(0)

#endif // _DEBUG

#if defined(_MSC_VER)
#define DEBUG_BREAK() __debugbreak()
#elif defined(__GNUC__) || defined(__clang__)
#include <signal.h>
#define DEBUG_BREAK() raise(SIGTRAP)
#else
#define DEBUG_BREAK() ((void)0)
#endif

#ifdef _DEBUG
#define __ENSURE_IMPLEMENTATION(condition, LOGGER, ...)	\
 do { \
        if (!(condition)) { \
            LOGGER("Throw: {}", __VA_ARGS__); \
            DEBUG_BREAK(); \
        } \
    } while (0)
#else
#define __ENSURE_IMPLEMENTATION(condition, LOGGER, ...) \
 do { \
        if (!(condition)) { \
            LOGGER("Throw: {}", __VA_ARGS__); \
        } \
    } while (0)
#endif

#define ENSURE(condition, ...) __ENSURE_IMPLEMENTATION(condition, LOG_CRITICAL, __VA_ARGS__)
#define THROW(...) ENSURE(false, __VA_ARGS__)

#ifdef _DEBUG
#define __ASSERT_IMPLEMENTATION(condition, LOGGER, ...)	\
 do { \
        if (!(condition)) { \
           LOGGER("Assertion: {}", __VA_ARGS__); \
           assert(0); \
        } \
    } while (0)
#else
#define __ASSERT_IMPLEMENTATION(condition, LOGGER, ...) \
 do { \
        if (!(condition)) { \
            LOGGER("Assertion: {}", __VA_ARGS__); \
           std::abort();   \
        } \
    } while (0)
#endif

#define ASSERT(condition, ...) __ASSERT_IMPLEMENTATION(condition, LOG_CRITICAL, __VA_ARGS__)

#ifdef _Win32
#include <comdef.h>
#include <Utility/Utility.h>
namespace fmt 
{
    template <>
    struct formatter<HRESULT> : public formatter<String> 
    {
        template <typename FormatContext>
        auto format(const HRESULT& hr, FormatContext& ctx) 
        {
            _com_error err(hr);
            std::wstring msg = err.ErrorMessage();
            String result = WStringToString(msg);
            return formatter<String>::format(result, ctx);
        }
    };

    template <>
    struct formatter<Vector2> : public formatter<String>
    {
        template <typename FormatContext>
        auto format(const Vector2& v, FormatContext& ctx)
        {
            String result = String("X: " + std::to_string(v.x)+ ", Y: " + std::to_string(v.y));
            return formatter<String>::format(result, ctx);
        }
    };

    template <>
    struct formatter<Vector3> : public formatter<String>
    {
        template <typename FormatContext>
        auto format(const Vector3& v, FormatContext& ctx)
        {
            String result = String("X: " + std::to_string(v.x) + ", Y: " + std::to_string(v.y) + ", Z: " + std::to_string(v.z));
            return formatter<String>::format(result, ctx);
        }
    };
}
#endif // Win32