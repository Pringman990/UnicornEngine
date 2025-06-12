#pragma once
#include <Core.h>
#include "IRHIDevice.h"

class IRHIRenderer
{
public:
	virtual ~IRHIRenderer() = default;

	/// <summary>
	/// Called directly when the renderer creates the backend
	/// </summary>
	/// <returns></returns>
	virtual bool Init() = 0;

	/// <summary>
	/// Used to clear backbuffer and setup buffers before anything gets submited
	/// </summary>
	/// <returns></returns>
	virtual void PreRender() = 0;

	/// <summary>
	/// Waits for the GPU and swaps data
	/// </summary>
	/// <returns></returns>
	virtual void Present() = 0;


	virtual IRHIDevice* GetDevice() const = 0;
};