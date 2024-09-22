#pragma once

class GenericApplication;

class EngineLoop
{
public:
	EngineLoop();
	~EngineLoop();

	bool Init();
	void Update();

	void RequestExit();
	bool EngineLoopClose() const;
private:

	bool mShouldExit;

	GenericApplication* mGenericApplication;
};