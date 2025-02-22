#pragma once

class MemoryDebugger
{
public:
	MemoryDebugger();
	~MemoryDebugger();

	static void StartStackMemoryTracking(bool showFullMemoryTrack, bool showAllAllocations);
	static void StopStackMemoryTracking();
	static void PauseStackMemoryTracking(bool Pause);

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