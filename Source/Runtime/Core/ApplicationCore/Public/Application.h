#pragma once
#include <Generic/GenericApplication.h>

class Application
{
public:
	/// <summary>
	/// Pointer can be null if used before EngineLoop has run Init
	/// </summary>
	static GenericApplication* GetApp()
	{
		return mGenericApplication;
	}

	static GenericApplication* _CreateApp();

	static void Shutdown();

private:
	Application();
	~Application();
private:
	static GenericApplication* mGenericApplication;
};