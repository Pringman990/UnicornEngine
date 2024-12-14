#pragma once
#include <Singleton.h>
#include <Generic/GenericApplication.h>

class Application : public Singleton<Application>
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
	friend class Singleton<Application>;
	Application();
	~Application();
private:
	GenericApplication* mGenericApplication;
};