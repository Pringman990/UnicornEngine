#pragma once
#include "LogCommon.h"

class ILogSink
{
public:
    virtual ~ILogSink() = default;

    virtual void Write(const LogMessage& message) = 0;

};
