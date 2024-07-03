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
			fps = frameCount / static_cast<int>(elapsedTime);

			// Reset frame count and elapsed time
			frameCount = 0;
			elapsedTime = 0.0f;
		}
	}

	const float GetDeltaTime() const  {
		return myDeltaTime;
	}

	const int GetFps() const
	{
		return fps;
	};

private:
	std::chrono::high_resolution_clock::time_point myLastTime = {};
	float myDeltaTime = 0;

	int fps = 0;
	int frameCount = 0;
	float elapsedTime = 0.0f;
};