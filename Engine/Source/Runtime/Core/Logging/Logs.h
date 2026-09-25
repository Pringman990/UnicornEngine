#pragma once
#include "Logger.h"
#include "../ServiceRegistry.h"

#define LOG_INFO(fmt, ...) \
Services::Get<Logger>().Log(LogLevel::Info, std::source_location::current(), fmt __VA_OPT__(,) __VA_ARGS__)

#define LOG_WARNING(fmt, ...) \
Services::Get<Logger>().Log(LogLevel::Warning, std::source_location::current(), fmt __VA_OPT__(,) __VA_ARGS__)