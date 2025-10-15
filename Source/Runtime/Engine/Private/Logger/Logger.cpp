#include "Logger/Logger.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/dist_sink.h>

namespace
{
	ENGINE_API SharedPtr<spdlog::logger>& GetLogger()
	{
		static SharedPtr<spdlog::logger> logger;
		return logger;
	}

	class CustomSink : public spdlog::sinks::sink
	{
	public:
		CustomSink()
		{
			// Initialize the base logger
			default_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		}

		~CustomSink()
		{
			default_sink.reset();
		}

		void log(const spdlog::details::log_msg& msg) override
		{
			std::string pattern;

			// Check the log level and apply the appropriate pattern
			switch (msg.level) {
			case spdlog::level::critical:
				pattern = "%^[%T][%l][%s:%#] %!: %v %$";
				break;
			case spdlog::level::info:
				pattern = "[%T][%^%l%$]: %v";
				break;
			default:
				pattern = "[%T][%^%l%$][%s:%#] %!: %v";
				break;
			}

			// Set the pattern for the base sink
			default_sink->set_pattern(pattern);

			// Log the message using the base sink
			default_sink->log(msg);
		}

		void flush() override {
			default_sink->flush();
		}

		void set_pattern(const std::string& /*pattern*/) override {
			// Override set_pattern if needed, but this method is currently not used
		}

		void set_formatter(std::unique_ptr<spdlog::formatter> /*sink_formatter*/) override {
			// Custom formatter logic if needed
		}

	private:
		SharedPtr<spdlog::sinks::stdout_color_sink_mt> default_sink;
	};
}

namespace Logger
{
	ENGINE_API SharedPtr<spdlog::logger>& GetMain()
	{
		return GetLogger();
	}

	ENGINE_API void Init()
	{
		_PAUSE_TRACK_MEMORY(true);

		auto consoleCustomSink = MakeShared<CustomSink>();

		GetLogger() = spdlog::basic_logger_mt("Main", "../Logs/core_log.txt");

		auto main = MakeShared<spdlog::sinks::dist_sink_mt>();
		main->add_sink(GetLogger()->sinks()[0]);
#ifdef _DEBUG
		main->add_sink(consoleCustomSink);
#endif // _DEBUG

		GetLogger()->sinks().clear();
		GetLogger()->sinks().push_back(main);

		spdlog::set_level(spdlog::level::trace);

		_PAUSE_TRACK_MEMORY(false);
	}

	ENGINE_API void Cleanup()
	{
		spdlog::apply_all([](SharedPtr<spdlog::logger> logger)
			{
				logger->flush();
			});

		spdlog::drop("Main");
		spdlog::drop_all();
		spdlog::shutdown();
	}
}
