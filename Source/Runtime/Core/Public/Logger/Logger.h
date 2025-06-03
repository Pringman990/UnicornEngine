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

//User Logs
#define LOG_INFO(...) \
    _PAUSE_TRACK_MEMORY(true);\
    ::Logger::Get()->GetClient()->info(__VA_ARGS__);\
    _PAUSE_TRACK_MEMORY(false)

#define LOG_WARNING(...)\
    _PAUSE_TRACK_MEMORY(true); \
    ::Logger::Get()->GetClient()->warn(__VA_ARGS__); \
    _PAUSE_TRACK_MEMORY(false)

#define LOG_ERROR(...)\
    _PAUSE_TRACK_MEMORY(true); \
    ::Logger::Get()->GetClient()->error(__VA_ARGS__); \
    _PAUSE_TRACK_MEMORY(false)

#define LOG_CRITICAL(...) \
    _PAUSE_TRACK_MEMORY(true); \
    ::Logger::Get()->GetClient()->critical(__VA_ARGS__); \
    _PAUSE_TRACK_MEMORY(false)

//Core Logs
#define _LOG_CORE_INFO(...) \
    _PAUSE_TRACK_MEMORY(true); \
    ::Logger::Get()->GetCore()->info(__VA_ARGS__); \
    _PAUSE_TRACK_MEMORY(false)

#define _LOG_CORE_WARNING(...) \
    _PAUSE_TRACK_MEMORY(true); \
    ::Logger::Get()->GetCore()->warn(__VA_ARGS__); \
    _PAUSE_TRACK_MEMORY(false)

#define _LOG_CORE_ERROR(...) \
    _PAUSE_TRACK_MEMORY(true); \
    ::Logger::Get()->GetCore()->error(__VA_ARGS__); \
    _PAUSE_TRACK_MEMORY(false)

#define _LOG_CORE_CRITICAL(...) \
    _PAUSE_TRACK_MEMORY(true); \
    ::Logger::Get()->GetCore()->critical(__VA_ARGS__); \
    _PAUSE_TRACK_MEMORY(false)

//Engine Logs
#define _LOG_ENGINE_INFO(...) \
    _PAUSE_TRACK_MEMORY(true); \
    ::Logger::Get()->GetEngine()->info(__VA_ARGS__); \
    _PAUSE_TRACK_MEMORY(false)

#define _LOG_ENGINE_WARNING(...) \
    _PAUSE_TRACK_MEMORY(true); \
    ::Logger::Get()->GetEngine()->warn(__VA_ARGS__); \
    _PAUSE_TRACK_MEMORY(false)

#define _LOG_ENGINE_ERROR(...) \
    _PAUSE_TRACK_MEMORY(true); \
    ::Logger::Get()->GetEngine()->error(__VA_ARGS__); \
    _PAUSE_TRACK_MEMORY(false)

#define _LOG_ENGINE_CRITICAL(...) \
    _PAUSE_TRACK_MEMORY(true); \
    ::Logger::Get()->GetEngine()->critical(__VA_ARGS__); \
    _PAUSE_TRACK_MEMORY(false)

//Renderer Logs
#define _LOG_RENDERER_INFO(...) \
    _PAUSE_TRACK_MEMORY(true); \
    ::Logger::Get()->GetRenderer()->info(__VA_ARGS__); \
    _PAUSE_TRACK_MEMORY(false)

#define _LOG_RENDERER_WARNING(...) \
    _PAUSE_TRACK_MEMORY(true); \
    ::Logger::Get()->GetRenderer()->warn(__VA_ARGS__); \
    _PAUSE_TRACK_MEMORY(false)

#define _LOG_RENDERER_ERROR(...) \
    _PAUSE_TRACK_MEMORY(true); \
    ::Logger::Get()->GetRenderer()->error(__VA_ARGS__); \
    _PAUSE_TRACK_MEMORY(false)

#define _LOG_RENDERER_CRITICAL(...) \
    _PAUSE_TRACK_MEMORY(true); \
    ::Logger::Get()->GetRenderer()->critical(__VA_ARGS__); \
    _PAUSE_TRACK_MEMORY(false)

//Editor Logs
#define _LOG_EDITOR_INFO(...)\
    _PAUSE_TRACK_MEMORY(true); \
    ::Logger::Get()->GetEditor()->info(__VA_ARGS__); \
    _PAUSE_TRACK_MEMORY(false)

#define _LOG_EDITOR_WARNING(...) \
    _PAUSE_TRACK_MEMORY(true); \
    ::Logger::Get()->GetEditor()->warn(__VA_ARGS__); \
    _PAUSE_TRACK_MEMORY(false)

#define _LOG_EDITOR_ERROR(...) \
    _PAUSE_TRACK_MEMORY(true); \
    ::Logger::Get()->GetEditor()->error(__VA_ARGS__); \
    _PAUSE_TRACK_MEMORY(false)

#define _LOG_EDITOR_CRITICAL(...) \
    _PAUSE_TRACK_MEMORY(true); \
    ::Logger::Get()->GetEditor()->critical(__VA_ARGS__); \
    _PAUSE_TRACK_MEMORY(false)

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

#ifdef _DEBUG
#define ENSURE(condition, message)										\
 do {														    \
        if (!(condition)) {                                     \
            LOG_CRITICAL("Assertion failed: {}, File: {}, Line: {}",				\
						message,								\
                        __FILE__,						\
                        __LINE__ );				\
            __debugbreak(); /* Triggers a breakpoint in VS */   \
        }                                                       \
    } while (0)

#define _ENSURE_CORE(condition, message)								   \
 do {															   \
        if (!(condition)) {                                        \
           _LOG_CORE_CRITICAL("Assertion failed: {}, File: {}, Line: {}",				\
						message,								\
                        __FILE__,						\
                        __LINE__ );					\
            __debugbreak(); /* Triggers a breakpoint in VS */      \
        }                                                          \
    } while (0)

#define _ENSURE_ENGINE(condition, message)								   \
 do {															   \
        if (!(condition)) {                                        \
           _LOG_ENGINE_CRITICAL("Assertion failed: {}, File: {}, Line: {}",				\
						message,								\
                        __FILE__,						\
                        __LINE__ );				\
            __debugbreak(); /* Triggers a breakpoint in VS */      \
        }                                                          \
    } while (0)

#define _ENSURE_RENDERER(condition, message)								   \
 do {															   \
        if (!(condition)) {                                        \
           _LOG_RENDERER_CRITICAL("Assertion failed: {}, File: {}, Line: {}",				\
						message,								\
                        __FILE__,						\
                        __LINE__ );				\
            __debugbreak(); /* Triggers a breakpoint in VS */      \
        }                                                          \
    } while (0)

#define _ENSURE_EDITOR(condition, message)								   \
 do {															   \
        if (!(condition)) {                                        \
           _LOG_EDITOR_CRITICAL("Assertion failed: {}, File: {}, Line: {}",				\
						message,								\
                        __FILE__,						\
                        __LINE__ );				\
            __debugbreak(); /* Triggers a breakpoint in VS */      \
        }                                                          \
    } while (0)
#else
#define ENSURE(cond, msg) ((void)0)
#define _ENSURE_CORE(cond, msg) ((void)0)
#define _ENSURE_ENGINE(cond, msg) ((void)0)
#define _ENSURE_RENDERER(cond, msg) ((void)0)
#define _ENSURE_EDITOR(cond, msg) ((void)0)
#endif

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