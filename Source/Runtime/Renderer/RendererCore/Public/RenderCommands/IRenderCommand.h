#pragma once

class IRenderCommand
{
public:
	IRenderCommand() {};
	virtual ~IRenderCommand() {};

	virtual void Execute() = 0;

private:
	
};