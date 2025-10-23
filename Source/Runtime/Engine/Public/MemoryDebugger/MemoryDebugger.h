#pragma once
#include <Core.h>
#include "EngineDefines.h"

class MemoryDebugger
{
public:
	ENGINE_API MemoryDebugger();
	ENGINE_API ~MemoryDebugger();

	ENGINE_API static void StartStackMemoryTracking(bool showFullMemoryTrack, bool showAllAllocations);
	ENGINE_API static void StopStackMemoryTracking();
	ENGINE_API static void PauseStackMemoryTracking(bool Pause);

private:

};

#ifdef _MEMORY_DEBUG
#define _TRACK_MEMORY(showFullMemoryTrack, showAllAllocations) \
	MemoryDebugger::StartStackMemoryTracking(showFullMemoryTrack, showAllAllocations);

#define _STOP_TRACK_MEMORY() \
	MemoryDebugger::StopStackMemoryTracking();

#define _PAUSE_TRACK_MEMORY(Pause) \
	MemoryDebugger::PauseStackMemoryTracking(Pause);

#else
#define _TRACK_MEMORY(showFullMemoryTrack, showAllAllocations) ((void)0)

#define _STOP_TRACK_MEMORY() ((void)0)

#define _PAUSE_TRACK_MEMORY(Pause) ((void)0)
#endif // _MEMORY_DEBUG