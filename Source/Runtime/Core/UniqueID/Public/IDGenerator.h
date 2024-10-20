#pragma once
#include "StandardTypes.h"
#include <atomic>

class IDGenerator
{
public:
	static uint64 Generator64ID();

private:
	IDGenerator() {};
	~IDGenerator() {};
private:
	static uint64 GetTimestamp();
private:
	static std::atomic<uint32> mCounter;
	static const uint32 mSessionID;

};