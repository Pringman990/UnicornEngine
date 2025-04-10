#pragma once
#include "Application/Generic/IWindow.h"
#include "EventDispatcher/Notifier.h"

class GenericApplication
{
public:
	GenericApplication();
	virtual ~GenericApplication();

	static GenericApplication* Create();

	virtual bool Init();
	virtual void Update();

	virtual IWindowInfo& GetWindowInfo();

	MultiNotifier OnApplicationRequestExist;
protected:
};