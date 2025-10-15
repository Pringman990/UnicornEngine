#pragma once
#include <Core.h>
#include <Subsystem/EngineSubsystem.h>
#include "Generic/GenericApplication.h"

/*
* Engine subsystem
*/
class Application
{
	friend struct subsystem::SubsystemDescriptor;
public:
	/// <summary>
	/// Pointer can be null if used before EngineLoop has run Init
	/// </summary>
	inline GenericApplication* GetApplication()
	{
		return mGenericApplication;
	}

	ENGINE_API GenericApplication* _CreateApplication();

private:
	Application();
	~Application();
private:
	GenericApplication* mGenericApplication;
};