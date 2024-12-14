#pragma once
#include <Singleton.h>

#include <memory>
#include <string>
#include <spdlog/spdlog.h>
#include <MemoryDebugger.h>

class Logger : public Singleton<Logger>
{
public:
	void Init();

	inline std::shared_ptr<spdlog::logger>& GetCore()  { return mCoreLogger;};
	inline std::shared_ptr<spdlog::logger>& GetClient() { return mClientLogger; };
	inline std::shared_ptr<spdlog::logger>& GetEditor() { return mEditorLogger; };
	inline std::shared_ptr<spdlog::logger>& GetRenderer() { return mRendererLogger; };

private:
	friend class Singleton<Logger>;
	Logger();
	~Logger() override;
private:

	std::shared_ptr<spdlog::logger> mCoreLogger;
	std::shared_ptr<spdlog::logger> mRendererLogger;
	std::shared_ptr<spdlog::logger> mEditorLogger;
	std::shared_ptr<spdlog::logger> mClientLogger;
};

#ifdef _DEBUG

//User Logs
#define LOG_INFO(...) \
    _PAUSE_TRACK_MEMORY(true);\
    ::Logger::GetInstance()->GetClient()->info(__VA_ARGS__);\
    _PAUSE_TRACK_MEMORY(false)

#define LOG_WARNING(...)\
    _PAUSE_TRACK_MEMORY(true); \
    ::Logger::GetInstance()->GetClient()->warn(__VA_ARGS__); \
    _PAUSE_TRACK_MEMORY(false)

#define LOG_ERROR(...)\
    _PAUSE_TRACK_MEMORY(true); \
    ::Logger::GetInstance()->GetClient()->error(__VA_ARGS__); \
    _PAUSE_TRACK_MEMORY(false)

#define LOG_CRITICAL(...) \
    _PAUSE_TRACK_MEMORY(true); \
    ::Logger::GetInstance()->GetClient()->critical(__VA_ARGS__); \
    _PAUSE_TRACK_MEMORY(false)

//Core Logs
#define _LOG_CORE_INFO(...) \
    _PAUSE_TRACK_MEMORY(true); \
    ::Logger::GetInstance()->GetCore()->info(__VA_ARGS__); \
    _PAUSE_TRACK_MEMORY(false)

#define _LOG_CORE_WARNING(...) \
    _PAUSE_TRACK_MEMORY(true); \
    ::Logger::GetInstance()->GetCore()->warn(__VA_ARGS__); \
    _PAUSE_TRACK_MEMORY(false)

#define _LOG_CORE_ERROR(...) \
    _PAUSE_TRACK_MEMORY(true); \
    ::Logger::GetInstance()->GetCore()->error(__VA_ARGS__); \
    _PAUSE_TRACK_MEMORY(false)

#define _LOG_CORE_CRITICAL(...) \
    _PAUSE_TRACK_MEMORY(true); \
    ::Logger::GetInstance()->GetCore()->critical(__VA_ARGS__); \
    _PAUSE_TRACK_MEMORY(false)

//Renderer Logs
#define _LOG_RENDERER_INFO(...) \
    _PAUSE_TRACK_MEMORY(true); \
    ::Logger::GetInstance()->GetRenderer()->info(__VA_ARGS__); \
    _PAUSE_TRACK_MEMORY(false)

#define _LOG_RENDERER_WARNING(...) \
    _PAUSE_TRACK_MEMORY(true); \
    ::Logger::GetInstance()->GetRenderer()->warn(__VA_ARGS__); \
    _PAUSE_TRACK_MEMORY(false)

#define _LOG_RENDERER_ERROR(...) \
    _PAUSE_TRACK_MEMORY(true); \
    ::Logger::GetInstance()->GetRenderer()->error(__VA_ARGS__); \
    _PAUSE_TRACK_MEMORY(false)

#define _LOG_RENDERER_CRITICAL(...) \
    _PAUSE_TRACK_MEMORY(true); \
    ::Logger::GetInstance()->GetRenderer()->critical(__VA_ARGS__); \
    _PAUSE_TRACK_MEMORY(false)

//Editor Logs
#define _LOG_EDITOR_INFO(...)\
    _PAUSE_TRACK_MEMORY(true); \
    ::Logger::GetInstance()->GetEditor()->info(__VA_ARGS__); \
    _PAUSE_TRACK_MEMORY(false)

#define _LOG_EDITOR_WARNING(...) \
    _PAUSE_TRACK_MEMORY(true); \
    ::Logger::GetInstance()->GetEditor()->warn(__VA_ARGS__); \
    _PAUSE_TRACK_MEMORY(false)

#define _LOG_EDITOR_ERROR(...) \
    _PAUSE_TRACK_MEMORY(true); \
    ::Logger::GetInstance()->GetEditor()->error(__VA_ARGS__); \
    _PAUSE_TRACK_MEMORY(false)

#define _LOG_EDITOR_CRITICAL(...) \
    _PAUSE_TRACK_MEMORY(true); \
    ::Logger::GetInstance()->GetEditor()->critical(__VA_ARGS__); \
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
#define _ENSURE_RENDERER(cond, msg) ((void)0)
#define _ENSURE_EDITOR(cond, msg) ((void)0)
#endif

#include <comdef.h>
#include <Utility.h>
namespace fmt {
    template <>
    struct formatter<HRESULT> : public formatter<std::string> {
        template <typename FormatContext>
        auto format(const HRESULT& hr, FormatContext& ctx) {
            _com_error err(hr);
            std::wstring msg = err.ErrorMessage();
            std::string result = WStringToString(msg);
            return formatter<std::string>::format(result, ctx); 
        }
    };
}