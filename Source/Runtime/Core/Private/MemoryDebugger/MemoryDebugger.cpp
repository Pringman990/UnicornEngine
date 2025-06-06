#include "MemoryDebugger/MemoryDebugger.h"
#include "MemoryTracker.h"

MemoryDebugger::MemoryDebugger()
{
}

MemoryDebugger::~MemoryDebugger()
{
}

void MemoryDebugger::StartStackMemoryTracking(bool ShowFullMemoryTrack, bool ShowAllAllocations)
{
	StartMemoryTracking({ ShowFullMemoryTrack, ShowAllAllocations });
}

void MemoryDebugger::StopStackMemoryTracking()
{
	StopMemoryTrackingAndPrint();
}

void MemoryDebugger::PauseStackMemoryTracking(bool Pause)
{
	PauseTracking(Pause);
}
