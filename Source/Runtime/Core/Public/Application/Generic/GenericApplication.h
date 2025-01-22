#pragma once
#include "Application/Generic/IWindow.h"

class GenericApplication
{
public:
	GenericApplication();
	virtual ~GenericApplication();

	static GenericApplication* Create();

	virtual bool Init();
	virtual void Update();

	virtual IWindowInfo& GetWindowInfo();

private:
};