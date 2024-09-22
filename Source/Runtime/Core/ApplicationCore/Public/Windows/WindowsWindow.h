#pragma once
#include "Generic/IWindow.h"

class WindowsWindow : public IWindow
{
public:
	WindowsWindow();
	~WindowsWindow();

	bool Init() override;

private:

};