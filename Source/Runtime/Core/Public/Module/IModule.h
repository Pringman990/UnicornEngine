#pragma once

class IModule
{
public:
	virtual ~IModule() {};

	virtual void Startup() = 0;
	virtual void Shutdown() = 0;

private:

};