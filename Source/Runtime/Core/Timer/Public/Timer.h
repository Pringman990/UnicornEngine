#pragma once
#include <chrono>
#include <mutex>
#include <TUMap.h>
#include <FString.h>

class Timer {
public:
	static Timer& GetInstance()
	{
		if (!mInstance)
			mInstance = new Timer();
		return *mInstance;
	}

	static void ShutDown()
	{
		delete mInstance;
		mInstance = nullptr;
	}

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

	void StartReading(FString aName)
	{
		std::lock_guard<std::mutex> lock(mReadingMutex);
		mStartedReadings[aName] = std::chrono::high_resolution_clock::now();
	}

	const float EndReading(FString aName, bool SaveAfterEnded = false)
	{
		std::lock_guard<std::mutex> lock(mReadingMutex);
		auto end = std::chrono::high_resolution_clock::now();
		auto it = mStartedReadings.Find(aName);
		if (it.IsValid())
		{
			float duration = std::chrono::duration<float>(end - it.Value()).count();
			mStartedReadings.Remove(aName);
			if (SaveAfterEnded)
				mSavedReadings[aName] = duration;

			return duration;
		}
		return 0.0f;
	}

	const float GetSavedReading(FString aName)
	{
		std::lock_guard<std::mutex> lock(mReadingMutex);
		auto it = mSavedReadings.Find(aName);
		if(it.IsValid())
		{
			return it.Value();
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
	Timer() {
		mLastTime = std::chrono::high_resolution_clock::now();
	}

	~Timer() {};
private:
	static Timer* mInstance;

	mutable std::mutex mReadingMutex;

	std::chrono::high_resolution_clock::time_point mLastTime = {};
	float mDeltaTime = 0;
	uint32_t mCurrentFps = 0;

	uint32_t mFrameCount = 0;
	float mElapsedTime = 0.0f;

	TUMap<FString, std::chrono::high_resolution_clock::time_point> mStartedReadings;
	TUMap<FString, float> mSavedReadings;
};

#define TIMER_START_READING(NAME) Timer::GetInstance().StartReading(NAME);
#define TIMER_END_READING(NAME) Timer::GetInstance().EndReading(NAME);