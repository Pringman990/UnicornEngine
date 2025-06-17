#pragma once
#include <EngineSubsystem.h>

#include <memory>
#include <string>
#include <spdlog/spdlog.h>
#include <MemoryDebugger/MemoryDebugger.h>
#include "StandardTypes/StandardTypes.h"

class Logger : public EngineSubsystem<Logger>
{
public:
	void Init();

	inline SharedPtr<spdlog::logger>& GetCore()  { return mCoreLogger;};
	inline SharedPtr<spdlog::logger>& GetEngine()  { return mEngineLogger;};
	inline SharedPtr<spdlog::logger>& GetClient() { return mClientLogger; };
	inline SharedPtr<spdlog::logger>& GetEditor() { return mEditorLogger; };
	inline SharedPtr<spdlog::logger>& GetRenderer() { return mRendererLogger; };

private:
	friend class EngineSubsystem<Logger>;
	Logger();
	~Logger() override;
private:

	SharedPtr<spdlog::logger> mCoreLogger;
	SharedPtr<spdlog::logger> mEngineLogger;
	SharedPtr<spdlog::logger> mRendererLogger;
	SharedPtr<spdlog::logger> mEditorLogger;
	SharedPtr<spdlog::logger> mClientLogger;
};

#ifdef _DEBUG

#define _LOGGER_STOP_MEMORY_TRACKING(LOGGER) \
    _PAUSE_TRACK_MEMORY(true);\
    LOGGER; \
    _PAUSE_TRACK_MEMORY(false)

//User Logs
#define LOG_INFO(...)       _LOGGER_STOP_MEMORY_TRACKING(::Logger::Get()->GetClient()->info(__VA_ARGS__))
#define LOG_WARNING(...)    _LOGGER_STOP_MEMORY_TRACKING(::Logger::Get()->GetClient()->warn(__VA_ARGS__))
#define LOG_ERROR(...)      _LOGGER_STOP_MEMORY_TRACKING(::Logger::Get()->GetClient()->error(__VA_ARGS__))
#define LOG_CRITICAL(...)   _LOGGER_STOP_MEMORY_TRACKING(::Logger::Get()->GetClient()->critical(__VA_ARGS__))

//Core Logs
#define _LOG_CORE_INFO(...)     _LOGGER_STOP_MEMORY_TRACKING(::Logger::Get()->GetCore()->info(__VA_ARGS__))
#define _LOG_CORE_WARNING(...)  _LOGGER_STOP_MEMORY_TRACKING(::Logger::Get()->GetCore()->warn(__VA_ARGS__))
#define _LOG_CORE_ERROR(...)    _LOGGER_STOP_MEMORY_TRACKING(::Logger::Get()->GetCore()->error(__VA_ARGS__))
#define _LOG_CORE_CRITICAL(...) _LOGGER_STOP_MEMORY_TRACKING(::Logger::Get()->GetCore()->critical(__VA_ARGS__))

//Engine Logs
#define _LOG_ENGINE_INFO(...)       _LOGGER_STOP_MEMORY_TRACKING(::Logger::Get()->GetEngine()->info(__VA_ARGS__))
#define _LOG_ENGINE_WARNING(...)    _LOGGER_STOP_MEMORY_TRACKING(::Logger::Get()->GetEngine()->warn(__VA_ARGS__))
#define _LOG_ENGINE_ERROR(...)      _LOGGER_STOP_MEMORY_TRACKING(::Logger::Get()->GetEngine()->error(__VA_ARGS__))
#define _LOG_ENGINE_CRITICAL(...)   _LOGGER_STOP_MEMORY_TRACKING(::Logger::Get()->GetEngine()->critical(__VA_ARGS__))

//Renderer Logs
#define _LOG_RENDERER_INFO(...)     _LOGGER_STOP_MEMORY_TRACKING(::Logger::Get()->GetRenderer()->info(__VA_ARGS__))
#define _LOG_RENDERER_WARNING(...)  _LOGGER_STOP_MEMORY_TRACKING(::Logger::Get()->GetRenderer()->warn(__VA_ARGS__))
#define _LOG_RENDERER_ERROR(...)    _LOGGER_STOP_MEMORY_TRACKING(::Logger::Get()->GetRenderer()->error(__VA_ARGS__))
#define _LOG_RENDERER_CRITICAL(...) _LOGGER_STOP_MEMORY_TRACKING(::Logger::Get()->GetRenderer()->critical(__VA_ARGS__))

//Editor Logs
#define _LOG_EDITOR_INFO(...)       _LOGGER_STOP_MEMORY_TRACKING(::Logger::Get()->GetEditor()->info(__VA_ARGS__))
#define _LOG_EDITOR_WARNING(...)    _LOGGER_STOP_MEMORY_TRACKING(::Logger::Get()->GetEditor()->warn(__VA_ARGS__))
#define _LOG_EDITOR_ERROR(...)      _LOGGER_STOP_MEMORY_TRACKING(::Logger::Get()->GetEditor()->error(__VA_ARGS__))
#define _LOG_EDITOR_CRITICAL(...)   _LOGGER_STOP_MEMORY_TRACKING(::Logger::Get()->GetEditor()->critical(__VA_ARGS__))

#else
#define LOG_INFO(...) (void)(0)
#define LOG_WARNING(...) (void)(0)
#define LOG_ERROR(...) (void)(0)
#define LOG_CRITICAL(...) (void)(0)

#define _LOG_CORE_INFO(...) (void)(0)
#define _LOG_CORE_WARNING(...) (void)(0)
#define _LOG_CORE_ERROR(...) (void)(0)
#define _LOG_CORE_CRITICAL(...)

#define _LOG_ENGINE_INFO(...) (void)(0)
#define _LOG_ENGINE_WARNING(...) (void)(0)
#define _LOG_ENGINE_ERROR(...) (void)(0)
#define _LOG_ENGINE_CRITICAL(...)

#define _LOG_RENDERER_INFO(...) (void)(0)
#define _LOG_RENDERER_WARNING(...) (void)(0)
#define _LOG_RENDERER_ERROR(...) (void)(0)
#define _LOG_RENDERER_CRITICAL(...) (void)(0)

#define _LOG_EDITOR_INFO(...) (void)(0)
#define _LOG_EDITOR_WARNING(...) (void)(0)
#define _LOG_EDITOR_ERROR(...) (void)(0)
#define _LOG_EDITOR_CRITICAL(...) (void)(0)
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
#define __ENSURE_IMPLEMENTATION(condition, LOGGER, ...)										\
 do {														    \
        if (!(condition)) {                                     \
            LOGGER("Throw: {}, File: {}, Line: {}",				\
						__VA_ARGS__,								\
                        __FILE__,						\
                        __LINE__ );				\
            DEBUG_BREAK();   \
        }                                                       \
    } while (0)
#else
#define __ENSURE_IMPLEMENTATION(condition, LOGGER, ...)										\
 do {														    \
        if (!(condition)) {                                     \
            LOGGER("Throw: {}, File: {}, Line: {}",				\
						__VA_ARGS__,								\
                        __FILE__,						\
                        __LINE__ );				\
        }                                                       \
    } while (0)
#endif

#define ENSURE(condition, ...) __ENSURE_IMPLEMENTATION(condition, LOG_CRITICAL, __VA_ARGS__)
#define _ENSURE_CORE(condition, ...) __ENSURE_IMPLEMENTATION(condition, _LOG_CORE_CRITICAL, __VA_ARGS__)
#define _ENSURE_ENGINE(condition, ...) __ENSURE_IMPLEMENTATION(condition, _LOG_ENGINE_CRITICAL, __VA_ARGS__)
#define _ENSURE_RENDERER(condition, ...) __ENSURE_IMPLEMENTATION(condition, _LOG_RENDERER_CRITICAL, __VA_ARGS__)
#define _ENSURE_EDITOR(condition, ...)	__ENSURE_IMPLEMENTATION(condition, _LOG_EDITOR_CRITICAL, __VA_ARGS__)

#define THROW(...) ENSURE(false, __VA_ARGS__)
#define _THROW_CORE(...) _ENSURE_CORE(false, __VA_ARGS__)
#define _THROW_ENGINE(...) _ENSURE_ENGINE(false, __VA_ARGS__)
#define _THROW_RENDERER(...) _ENSURE_RENDERER(false, __VA_ARGS__)
#define _THROW_EDITOR(...) _ENSURE_EDITOR(false, __VA_ARGS__)

#ifdef _DEBUG
#define __ASSERT_IMPLEMENTATION(condition, LOGGER, ...)										\
 do {														    \
        if (!(condition)) {                                     \
            LOGGER("Assertion: {}, File: {}, Line: {}",				\
						__VA_ARGS__,								\
                        __FILE__,						\
                        __LINE__ );				\
           assert(0);   \
        }                                                       \
    } while (0)
#else
#define __ASSERT_IMPLEMENTATION(condition, LOGGER, ...)										\
 do {														    \
        if (!(condition)) {                                     \
            LOGGER("Assertion: {}, File: {}, Line: {}",				\
						__VA_ARGS__,								\
                        __FILE__,						\
                        __LINE__ );				\
           std::abort();   \
        }                                                       \
    } while (0)
#endif

#define ASSERT(condition, ...) __ASSERT_IMPLEMENTATION(condition, LOG_CRITICAL, __VA_ARGS__)
#define _ASSERT_CORE(condition, ...) __ASSERT_IMPLEMENTATION(condition, _LOG_CORE_CRITICAL, __VA_ARGS__)
#define _ASSERT_ENGINE(condition, ...) __ASSERT_IMPLEMENTATION(condition, _LOG_ENGINE_CRITICAL, __VA_ARGS__)
#define _ASSERT_RENDERER(condition, ...) __ASSERT_IMPLEMENTATION(condition, _LOG_RENDERER_CRITICAL, __VA_ARGS__)
#define _ASSERT_EDITOR(condition, ...)	__ASSERT_IMPLEMENTATION(condition, _LOG_EDITOR_CRITICAL, __VA_ARGS__)

#ifdef _Win32
#include <comdef.h>
#include <Utility/Utility.h>
namespace fmt {
    template <>
    struct formatter<HRESULT> : public formatter<String> {
        template <typename FormatContext>
        auto format(const HRESULT& hr, FormatContext& ctx) {
            _com_error err(hr);
            std::wstring msg = err.ErrorMessage();
            String result = WStringToString(msg);
            return formatter<String>::format(result, ctx);
        }
    };
}
#endif // Win32