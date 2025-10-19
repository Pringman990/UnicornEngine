#pragma once

//Credit TGA
struct MemoryTrackingSettings
{
	// Enables storage of stack traces for all allocations. This comes at a significant performance cost, but makes it easier to track down memory leaks
	bool myShouldStoreStackTraces;
	// Enables tracking of all allocations, not just leaks. Prints a summary of top allocation stack traces if myShouldStoreStackTraces is enabled.
	bool myShouldTrackAllAllocations;
};

// Keeps track of memory allocations and deallocations when compiled with _DEBUG (normally defined in Debug).
// By default only leaks are tracked and stack traces are not stored. 
void StartMemoryTracking(const MemoryTrackingSettings& trackingSettings);
void StopMemoryTrackingAndPrint();
void PauseTracking(bool Pause);
