#pragma once
#include "../Core/Logging/ILogSink.h"

class ConsoleLogSink : public ILogSink
{
public:
    virtual void Write(const LogMessage& message) override;

private:
};
