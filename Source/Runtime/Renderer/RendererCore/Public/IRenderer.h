#pragma once
#include <StandardTypes.h>

class IRenderer
{
public:
	IRenderer() : mDrawCalls(0), mVsyncEnabled(true) {};
	virtual ~IRenderer() {};

	virtual void Shutdown() = 0;
	
	virtual bool Init() = 0;
	virtual void Update() = 0;

public:
	uint32 GetDrawCalls() const { return mDrawCalls; };

protected:
	void AddDrawCall() { mDrawCalls++; };

private:
	void ResetDrawCalls() {mDrawCalls = 0;};

protected:
	uint32 mDrawCalls;
	bool mVsyncEnabled;
};