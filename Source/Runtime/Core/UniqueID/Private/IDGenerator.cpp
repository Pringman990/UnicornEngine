#include "IDGenerator.h"

#include <chrono>

std::atomic<uint32> IDGenerator::mCounter(0);
const uint32_t IDGenerator::mSessionID = static_cast<uint32_t>(IDGenerator::GetTimestamp());

uint64 IDGenerator::Generator64ID()
{
	uint64 high = static_cast<uint64>(mSessionID) << 32;
	uint64 low = static_cast<uint64>(mCounter.fetch_add(1));

	uint64 fullID = high | low;
	
	//We use Zero to check for non set IDs
	if (fullID == 0)
		return Generator64ID();

	return fullID;
}

uint64 IDGenerator::GetTimestamp()
{
	return std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
}
