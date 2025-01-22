#include "MemoryDebugger/MemoryDebugger.h"
#include "MemoryTracker.h"

MemoryDebugger::MemoryDebugger()
{
}

MemoryDebugger::~MemoryDebugger()
{
}

void MemoryDebugger::StartStackMemoryTracking(bool showFullMemoryTrack, bool showAllAllocations)
{
	StartMemoryTracking({ showFullMemoryTrack, showAllAllocations });
}

void MemoryDebugger::StopStackMemoryTracking()
{
	StopMemoryTrackingAndPrint();
}

void MemoryDebugger::PauseStackMemoryTracking(bool Pause)
{
	PauseTracking(Pause);
}
