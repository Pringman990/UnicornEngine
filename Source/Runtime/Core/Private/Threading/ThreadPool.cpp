#include "pch.h"
#include "Threading/ThreadPool.h"

ThreadPool::ThreadPool()
	:
	mShouldStopAll(false)
{
    uint32 maxThreads = std::thread::hardware_concurrency();
    for (uint32 i = 0; i < maxThreads; i++)
    {
        mWorkers.emplace_back([this] 
            {
                while (true)
                {
                    std::function<void()> task;
                    std::function<void()> callback;
                    {
                        std::unique_lock<std::mutex> lock(mQueueMutex);
                        mCondition.wait(lock, [this] { return mShouldStopAll || !mTasks.empty(); });

                        if (mShouldStopAll && mTasks.empty())
                            return;

                        task = std::move(mTasks.front().first);
                        callback = std::move(mTasks.front().second);
                        mTasks.pop();
                    }
                    task();
                    if (callback)
                        callback();
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
