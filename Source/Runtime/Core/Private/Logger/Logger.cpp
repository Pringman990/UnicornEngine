#include "Logger/Logger.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/dist_sink.h>

class CustomSink : public spdlog::sinks::sink {
public:
    CustomSink() {
        // Initialize the base logger
        default_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    }

	~CustomSink()
	{
		default_sink.reset();
	}

    void log(const spdlog::details::log_msg& msg) override {
        std::string pattern;

        // Check the log level and apply the appropriate pattern
        switch (msg.level) {
        case spdlog::level::info:
            pattern = "%^[%T][%n][%l]:%$ %v";
            break;
        default:
            pattern = "%^[%T][%n][%l]: %v%$";
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
    std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> default_sink;
};

Logger::Logger()
{

}
Logger::~Logger()
{
	spdlog::apply_all([](std::shared_ptr<spdlog::logger> logger) {
		logger->flush();
		});

	spdlog::drop("Core");
	spdlog::drop("Engine");
	spdlog::drop("Editor");
	spdlog::drop("Renderer");
	spdlog::drop("Client");
	spdlog::drop_all();
	spdlog::shutdown();
}

void Logger::Init()
{
	_PAUSE_TRACK_MEMORY(true);

#ifdef _DEBUG
	auto consoleCustomSink = std::make_shared<CustomSink>();
#endif // _DEBUG

	mCoreLogger		= spdlog::basic_logger_mt("Core", "../Logs/core_log.txt");
	mEngineLogger	= spdlog::basic_logger_mt("Engine", "../Logs/engine_log.txt");
	mEditorLogger	= spdlog::basic_logger_mt("Editor", "../Logs/editor_log.txt");
	mRendererLogger = spdlog::basic_logger_mt("Renderer", "../Logs/renderer_log.txt");
	mClientLogger	= spdlog::basic_logger_mt("Client", "../Logs/client_log.txt");

	auto mainSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("../logs/full_log.txt", true);

	auto coreSink = std::make_shared<spdlog::sinks::dist_sink_mt>();
	coreSink->add_sink(mCoreLogger->sinks()[0]);
	coreSink->add_sink(mainSink);
#ifdef _DEBUG
	coreSink->add_sink(consoleCustomSink);
#endif // _DEBUG

	auto engineSink = std::make_shared<spdlog::sinks::dist_sink_mt>();
	engineSink->add_sink(mEngineLogger->sinks()[0]);
	engineSink->add_sink(mainSink);
#ifdef _DEBUG
	engineSink->add_sink(consoleCustomSink);
#endif // _DEBUG

	auto rendererSink = std::make_shared<spdlog::sinks::dist_sink_mt>();
	rendererSink->add_sink(mRendererLogger->sinks()[0]);
	rendererSink->add_sink(mainSink);
#ifdef _DEBUG
	rendererSink->add_sink(consoleCustomSink);
#endif // _DEBUG

	auto editorSink = std::make_shared<spdlog::sinks::dist_sink_mt>();
	editorSink->add_sink(mEditorLogger->sinks()[0]);
	editorSink->add_sink(mainSink);
#ifdef _DEBUG
	editorSink->add_sink(consoleCustomSink);
#endif // _DEBUG

	auto clientSink = std::make_shared<spdlog::sinks::dist_sink_mt>();
	clientSink->add_sink(mClientLogger->sinks()[0]);
	clientSink->add_sink(mainSink);
#ifdef _DEBUG
	clientSink->add_sink(consoleCustomSink);
#endif // _DEBUG

	mCoreLogger->sinks().clear();
	mCoreLogger->sinks().push_back(coreSink);

	mEngineLogger->sinks().clear();
	mEngineLogger->sinks().push_back(engineSink);

	mEditorLogger->sinks().clear();
	mEditorLogger->sinks().push_back(editorSink);

	mRendererLogger->sinks().clear();
	mRendererLogger->sinks().push_back(rendererSink);

	mClientLogger->sinks().clear();
	mClientLogger->sinks().push_back(clientSink);

	spdlog::set_level(spdlog::level::trace);

	_PAUSE_TRACK_MEMORY(false);
}
