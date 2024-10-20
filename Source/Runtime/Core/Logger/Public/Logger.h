#pragma once
#include <TSharedPtr.h>
#include <FString.h>
#include <spdlog/spdlog.h>

class Logger
{
public:
	static Logger& GetInstance() 
	{
		if (!_sInstance && !mHasShutDown)
		{
			_sInstance = new Logger();
			_sInstance->Init();
			_sInstance->mHasInit = true;
		}
		return *_sInstance;
	}

	static void Shutdown();

	void Init();

	inline TSharedPtr<spdlog::logger>& GetCore()  {return mCoreLogger;};
	inline TSharedPtr<spdlog::logger>& GetClient() { return mClientLogger; };
	inline TSharedPtr<spdlog::logger>& GetEditor() { return mEditorLogger; };
	inline TSharedPtr<spdlog::logger>& GetRenderer() { return mRendererLogger; };

private:
	Logger() : mHasInit(false) {};
	~Logger() {};
private:
	static Logger* _sInstance;

	TSharedPtr<spdlog::logger> mCoreLogger;
	TSharedPtr<spdlog::logger> mRendererLogger;
	TSharedPtr<spdlog::logger> mEditorLogger;
	TSharedPtr<spdlog::logger> mClientLogger;
	
	bool mHasInit;
	static bool mHasShutDown;
};

namespace fmt {
	template <>
	struct formatter<FString> : public formatter<std::string> {  // Using std::string as the base formatter
		template <typename FormatContext>
		auto format(const FString& s, FormatContext& ctx) {
			return formatter<std::string>::format(static_cast<std::string>(s), ctx); // Convert FString to std::string
		}
	};
}

#ifdef _DEBUG

#define LOG_INFO(...) ::Logger::GetInstance().GetClient()->info(__VA_ARGS__)
#define LOG_WARNING(...) ::Logger::GetInstance().GetClient()->warn(__VA_ARGS__)
#define LOG_ERROR(...) ::Logger::GetInstance().GetClient()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) ::Logger::GetInstance().GetClient()->critical(__VA_ARGS__)

#define LOG_CORE_INFO(...) ::Logger::GetInstance().GetCore()->info(__VA_ARGS__)
#define LOG_CORE_WARNING(...) ::Logger::GetInstance().GetCore()->warn(__VA_ARGS__)
#define LOG_CORE_ERROR(...) ::Logger::GetInstance().GetCore()->error(__VA_ARGS__)
#define LOG_CORE_CRITICAL(...) ::Logger::GetInstance().GetCore()->critical(__VA_ARGS__)

#define LOG_RENDERER_INFO(...) ::Logger::GetInstance().GetRenderer()->info(__VA_ARGS__)
#define LOG_RENDERER_WARNING(...) ::Logger::GetInstance().GetRenderer()->warn(__VA_ARGS__)
#define LOG_RENDERER_ERROR(...) ::Logger::GetInstance().GetRenderer()->error(__VA_ARGS__)
#define LOG_RENDERER_CRITICAL(...) ::Logger::GetInstance().GetRenderer()->critical(__VA_ARGS__)

#define LOG_EDITOR_INFO(...) ::Logger::GetInstance().GetEditor()->info(__VA_ARGS__)
#define LOG_EDITOR_WARNING(...) ::Logger::GetInstance().GetEditor()->warn(__VA_ARGS__)
#define LOG_EDITOR_ERROR(...) ::Logger::GetInstance().GetEditor()->error(__VA_ARGS__)
#define LOG_EDITOR_CRITICAL(...) ::Logger::GetInstance().GetEditor()->critical(__VA_ARGS__)

#else
#define LOG_INFO(...) (void)(0)
#define LOG_WARNING(...) (void)(0)
#define LOG_ERROR(...) (void)(0)
#define LOG_CRITICAL(...) (void)(0)

#define LOG_CORE_INFO(...) (void)(0)
#define LOG_CORE_WARNING(...) (void)(0)
#define LOG_CORE_ERROR(...) (void)(0)
#define LOG_CORE_CRITICAL(...)

#define LOG_RENDERER_INFO(...) (void)(0)
#define LOG_RENDERER_WARNING(...) (void)(0)
#define LOG_RENDERER_ERROR(...) (void)(0)
#define LOG_RENDERER_CRITICAL(...) (void)(0)

#define LOG_EDITOR_INFO(...) (void)(0)
#define LOG_EDITOR_WARNING(...) (void)(0)
#define LOG_EDITOR_ERROR(...) (void)(0)
#define LOG_EDITOR_CRITICAL(...) (void)(0)
#endif // _DEBUG
