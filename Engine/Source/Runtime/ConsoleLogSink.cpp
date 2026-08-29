//
// Created on 2026-07-15.
//
#include "pch.h"
#include "ConsoleLogSink.h"

void ConsoleLogSink::Write(const LogMessage& message)
{
    if (message.level == LogLevel::Fatal)
    {
        printf(
            "[%s] [%s: %u:%u] %s\n",
            ToString(message.level),
            message.location.file_name(),
            message.location.line(),
            message.location.column(),
            message.text.c_str()
        );
    }
    else
    {
        printf(
             "[%s] %s\n",
             ToString(message.level),
             message.text.c_str()
         );
    }
}
