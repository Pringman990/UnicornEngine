#pragma once

class IRenderingBackend
{
public:
	IRenderingBackend() = default;
	virtual ~IRenderingBackend() = default;

protected:
	virtual bool Init() = 0;
	virtual void PreRender() = 0;
	virtual void RenderToBackbuffer() = 0;
	virtual void Present() = 0;
private:
	friend class Renderer;
};