#pragma once

enum class LogLevel
{
    Trace,
    Info,
    Warning,
    Fatal
};

struct LogMessage
{
    LogLevel level;

    String text;

    std::source_location location;

    std::chrono::system_clock::time_point time;

    // TODO: look into storing callstack.
    //Optional<std::stacktrace> callstack;
};

inline const char* ToString(const LogLevel level)
{
    switch (level)
    {
    case LogLevel::Trace:
        return "Trace";
    case LogLevel::Info:
        return "Info";
    case LogLevel::Warning:
        return "Warning";
    case LogLevel::Fatal:
        return "Fatal";
    default:
        return "Unknown";
    }
}