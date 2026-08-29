#pragma once
#include "Core/ILogSink.h"

class ConsoleLogSink : public ILogSink
{
public:
    void Write(const LogMessage& message) override;

private:
};
