#pragma once
#include <Singleton.h>

#include <chrono>
#include <mutex>

class Timer : public Singleton<Timer> {
public:
	void Update() {
		auto currentTime = std::chrono::high_resolution_clock::now();
		mDeltaTime = std::chrono::duration<float>(currentTime - mLastTime).count();
		mLastTime = currentTime;

		mFrameCount++;
		mElapsedTime += mDeltaTime;

		// Calculate FPS every second
		if (mElapsedTime >= 1.0f)
		{
			// Calculate frames per second
			mCurrentFps = mFrameCount / static_cast<uint32_t>(mElapsedTime);

			// Reset frame count and elapsed time
			mFrameCount = 0;
			mElapsedTime = 0.0f;
		}
	}

	void StartReading(std::string aName)
	{
		std::lock_guard<std::mutex> lock(mReadingMutex);
		mStartedReadings[aName] = std::chrono::high_resolution_clock::now();
	}

	const float EndReading(std::string aName, bool SaveAfterEnded = false)
	{
		std::lock_guard<std::mutex> lock(mReadingMutex);
		auto end = std::chrono::high_resolution_clock::now();
		auto it = mStartedReadings.find(aName);
		if (it != mStartedReadings.end())
		{
			float duration = std::chrono::duration<float>(end - it->second).count();
			mStartedReadings.erase(aName);
			if (SaveAfterEnded)
				mSavedReadings[aName] = duration;

			return duration;
		}
		return 0.0f;
	}

	const float GetSavedReading(std::string aName)
	{
		std::lock_guard<std::mutex> lock(mReadingMutex);
		auto it = mSavedReadings.find(aName);
		if(it != mSavedReadings.end())
		{
			return it->second;
		}
		return 0.0f;
	}

	const float GetDeltaTime() const {
		return mDeltaTime;
	}

	const uint32_t GetFps() const
	{
		return mCurrentFps;
	};

private:
	friend class Singleton<Timer>;
	Timer() {
		mLastTime = std::chrono::high_resolution_clock::now();
	}

	~Timer() {};
private:
	//static Timer* mInstance;

	mutable std::mutex mReadingMutex;

	std::chrono::high_resolution_clock::time_point mLastTime = {};
	float mDeltaTime = 0;
	uint32_t mCurrentFps = 0;

	uint32_t mFrameCount = 0;
	float mElapsedTime = 0.0f;

	std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> mStartedReadings;
	std::unordered_map<std::string, float> mSavedReadings;
};

#define TIMER_START_READING(NAME) Timer::GetInstance()->StartReading(NAME);
#define TIMER_END_READING(NAME) Timer::GetInstance()->EndReading(NAME);