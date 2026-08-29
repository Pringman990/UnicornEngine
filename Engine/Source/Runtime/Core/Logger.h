#pragma once
#include "LogCommon.h"
#include "ILogSink.h"
#include "ServiceRegistry.h"
#include "fmt/std.h"

class Logger
{
    INIT_SERVICE(Logger)

public:

    template <typename... Args>
    void Log(LogLevel level, std::source_location location, fmt::format_string<Args...> format, Args&&... args)
    {
        LogMessage message;

        message.level = level;
        message.location = location;
        message.time = std::chrono::system_clock::now();

        message.text = fmt::format(format, std::forward<Args>(args)...);

        for (const auto& sink : mSinks)
        {
            sink->Write(message);
        }
    }

    template <typename T, typename... Args>
    T& AddSink(Args&&... args)
    {
        auto sink = MakeOwned<T>(std::forward<Args>(args)...);

        T& ref = *sink;

        mSinks.emplace_back(std::move(sink));

        return ref;
    }

    template<typename T>
    void RemoveSink()
    {
        auto it = std::find_if(mSinks.begin(), mSinks.end(),
            [](const auto& sink)
        {
            return dynamic_cast<T*>(sink.get()) != nullptr;
        });

        if (it != mSinks.end())
        {
            mSinks.erase(it);
        }
    }

private:
    List<OwnedPtr<ILogSink>> mSinks;
};
