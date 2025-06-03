#pragma once
#include <EngineSubsystem.h>
#include "Generic/GenericApplication.h"

class Application : public EngineSubsystem<Application>
{
public:
	/// <summary>
	/// Pointer can be null if used before EngineLoop has run Init
	/// </summary>
	inline GenericApplication* GetApplication()
	{
		return mGenericApplication;
	}

	GenericApplication* _CreateApplication();

private:
	friend class EngineSubsystem<Application>;
	Application();
	~Application();
private:
	GenericApplication* mGenericApplication;
};