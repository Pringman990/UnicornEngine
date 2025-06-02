#include "pch.h"
#include "Threading/ThreadPool.h"

ThreadPool::ThreadPool()
	:
	mShouldStopAll(false)
{
	uint32 maxThreads = std::thread::hardware_concurrency();
	for (uint32 i = 0; i < maxThreads; i++)
	{
		mWorkers.emplace_back([this]() {
			while (true) {
				Func<void()> task;
				{
					std::unique_lock<std::mutex> lock(this->mQueueMutex);
					this->mCondition.wait(lock, [this]() { return this->mShouldStopAll || !this->mTasks.empty(); });

					if (this->mShouldStopAll && this->mTasks.empty()) return;  // Exit if stopping
					task = std::move(this->mTasks.front());
					this->mTasks.pop();
				}
				task();
			}
			});
	}
}

ThreadPool::~ThreadPool()
{
	{
		std::unique_lock<std::mutex> lock(mQueueMutex);
		mShouldStopAll = true;
	}
	mCondition.notify_all();
	for (std::thread& worker : mWorkers) {
		worker.join();
	}
}
