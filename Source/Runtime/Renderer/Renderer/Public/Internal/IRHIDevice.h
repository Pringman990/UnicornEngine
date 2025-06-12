#pragma once
#include <Core.h>

class IRHIDevice
{
public:
	virtual ~IRHIDevice() = default;

	virtual bool Create() = 0;

	/// <summary>
	/// Used for external libraries like ImGUI that needs the actual device from the graphics API
	/// </summary>
	/// <returns></returns>
	virtual void* GetRawDevice() = 0;
};