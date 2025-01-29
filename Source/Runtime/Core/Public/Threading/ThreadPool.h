#pragma once
#include <Singleton.h>
#include <mutex>
#include <queue>

class ThreadPool : public Singleton<ThreadPool>
{
public:
	template<typename Func, typename Callback = std::nullptr_t>
	void Enqueue(Func aTask, Callback aCallback = nullptr)
	{
		{
			std::unique_lock<std::mutex> lock(mQueueMutex);
			mTasks.emplace(
				std::function<void()>(std::forward<Func>(aTask)),
				aCallback ? std::function<void()>(std::forward<Callback>(aCallback)) : std::function<void()>()
			);
		}
		mCondition.notify_one();
	}

private:
	friend class Singleton<ThreadPool>;
	ThreadPool();
	~ThreadPool();
private:
	std::vector<std::thread> mWorkers;
	std::queue<std::pair<std::function<void()>, std::function<void()>>> mTasks;

	std::mutex mQueueMutex;
	std::condition_variable mCondition;
	bool mShouldStopAll;
};

