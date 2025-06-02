#pragma once
#include <Logger/Logger.h>

struct IWindowInfo
{
	uint32 windowWidth = 1200;
	uint32 windowHeight = 720;
	uint32 viewportWidth = 0;
	uint32 viewportHeight = 0;

	const char* name = "Unicorn";
};

class IWindow
{
public:
	IWindow() {};
	~IWindow() {};

	virtual bool Init() = 0;

private:

};