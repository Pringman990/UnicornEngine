#pragma once
#include <chrono>

class Timer{
public:
	Timer() {
		myLastTime = std::chrono::high_resolution_clock::now();
	}

	~Timer() {};

	void Update() {
		auto currentTime = std::chrono::high_resolution_clock::now();
		myDeltaTime = std::chrono::duration<float>(currentTime - myLastTime).count();
		myLastTime = currentTime;

		frameCount++;
		elapsedTime += myDeltaTime;
		// Calculate FPS every second
		if (elapsedTime >= 1.0f)
		{
			// Calculate frames per second
			fps = static_cast<float>(frameCount) / elapsedTime;

			// Reset frame count and elapsed time
			frameCount = 0;
			elapsedTime = 0.0f;
		}
	}

	const float GetDeltaTime() const  {
		return myDeltaTime;
	}

	const float GetFps() const
	{
		return fps;
	};

private:
	std::chrono::high_resolution_clock::time_point myLastTime = {};
	float myDeltaTime = 0;

	float fps = 0.0f;
	int frameCount = 0;
	float elapsedTime = 0.0f;
};