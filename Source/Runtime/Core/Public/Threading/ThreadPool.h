#pragma once
#include <EngineSubsystem.h>
#include <mutex>
#include <queue>
#include <functional>
#include <future>

class ThreadPool : public EngineSubsystem<ThreadPool>
{
public:
	template <typename TaskFunc, typename CallbackFunc>
	void Enqueue(TaskFunc&& taskFunc, CallbackFunc&& callbackFunc) {
		auto task = MakeShared<std::packaged_task<typename std::invoke_result<TaskFunc>::type()>>(
			std::forward<TaskFunc>(taskFunc)
		);

		// Capture the callback and wrap it
		auto wrappedCallback = [task, callbackFunc, this]() {
			auto result = task->get_future().get();
			// Add the callback to the main thread's callback queue
			{
				std::unique_lock<std::mutex> lock(mCallbackMutex);
				mCallbackQueue.push([callbackFunc, result]() { callbackFunc(result); });
			}
			mCondition.notify_one();  // Notify main thread to process the callback
			};

		{
			std::unique_lock<std::mutex> lock(mQueueMutex);
			if (mShouldStopAll) {
				throw std::runtime_error("enqueue on stopped ThreadPool");
			}
			mTasks.push([task, wrappedCallback]() {
				(*task)();  // Execute the task
				wrappedCallback();  // Add callback to the main thread's queue
				});
		}

		mCondition.notify_one();
	}

	void ProcessCallbacks()
	{
		std::unique_lock<std::mutex> lock(mCallbackMutex);
		while (!mCallbackQueue.empty()) {
			auto callback = std::move(mCallbackQueue.front());
			mCallbackQueue.pop();
			callback();
		}
	}

private:
	friend class EngineSubsystem<ThreadPool>;
	ThreadPool();
	~ThreadPool();
private:
	Vector<std::thread> mWorkers;
	std::queue<Func<void()>> mTasks;
	std::queue<Func<void()>> mCallbackQueue;
	std::mutex mQueueMutex;
	std::mutex mCallbackMutex;
	std::condition_variable mCondition;
	bool mShouldStopAll;
};

