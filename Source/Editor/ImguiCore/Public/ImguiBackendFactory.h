#pragma once
#include "IImguiBackend.h"

class ImguiBackendFactory
{
public:

	static IImguiBackend* CreateBackend();

private:
	ImguiBackendFactory() = delete;
	~ImguiBackendFactory() = delete;
};