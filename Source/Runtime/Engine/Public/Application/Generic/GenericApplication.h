#pragma once
#include "Application/Generic/IWindow.h"
#include "EventDispatcher/Notifier.h"

class GenericApplication
{
public:
	GenericApplication();
	virtual ~GenericApplication();

	ENGINE_API static GenericApplication* Create();

	ENGINE_API virtual bool Init();
	ENGINE_API virtual void Update();

	ENGINE_API virtual IWindowInfo& GetWindowInfo();

	MultiNotifier OnApplicationRequestExist;

	//Window Events
	MultiNotifierArgs<int32, int32> OnWindowResizeEvent;
protected:
};