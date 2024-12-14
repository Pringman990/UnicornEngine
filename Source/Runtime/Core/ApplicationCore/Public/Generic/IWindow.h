#pragma once
#include <Logger.h>

struct IWindowInfo
{
	uint32_t windowWidth = 1200;
	uint32_t windowHeight = 720;
	uint32_t viewportWidth = 0;
	uint32_t viewportHeight = 0;

	const wchar_t* name = L"Unicorn";
};

class IWindow
{
public:
	IWindow() {};
	~IWindow() {};

	virtual bool Init() = 0;

private:

};