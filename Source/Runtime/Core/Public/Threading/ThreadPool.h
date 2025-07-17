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
	void Enqueue(TaskFunc&& taskFunc, CallbackFunc&& callbackFunc)
	{
        using ReturnType = typename std::invoke_result<TaskFunc>::type;

        auto task = MakeShared<std::packaged_task<ReturnType()>>(
            std::forward<TaskFunc>(taskFunc)
        );

        // Special handling for void-returning taskFunc
        if constexpr (std::is_void_v<ReturnType>)
        {
            auto wrappedCallback = [task, callbackFunc, this]() mutable
                {
                    task->get_future().get(); // Wait for task to finish, no result
                    {
                        std::unique_lock<std::mutex> lock(mCallbackMutex);
                        mCallbackQueue.push(
                            [callbackFunc]() { callbackFunc(); } // Call without argument
                        );
                    }
                    mCondition.notify_one();
                };

            {
                std::unique_lock<std::mutex> lock(mQueueMutex);
                if (mShouldStopAll)
                    _THROW_CORE("enqueue on stopped ThreadPool");

                mTasks.push([task, wrappedCallback]() mutable
                    {
                        (*task)();        // Run task
                        wrappedCallback(); // Queue callback
                    });
            }
        }
        else // Non-void ReturnType
        {
            auto wrappedCallback = [task, callbackFunc, this]() mutable
                {
                    auto result = task->get_future().get(); // Wait & get result
                    {
                        std::unique_lock<std::mutex> lock(mCallbackMutex);
                        mCallbackQueue.push(
                            [callbackFunc, result = std::move(result)]() mutable
                            {
                                callbackFunc(std::move(result)); // Call with result
                            }
                        );
                    }
                    mCondition.notify_one();
                };

            {
                std::unique_lock<std::mutex> lock(mQueueMutex);
                if (mShouldStopAll)
                    _THROW_CORE("enqueue on stopped ThreadPool");

                mTasks.push([task, wrappedCallback]() mutable
                    {
                        (*task)();
                        wrappedCallback();
                    });
            }
        }

        mCondition.notify_one();
	}

	void ProcessCallbacks()
	{
		std::unique_lock<std::mutex> lock(mCallbackMutex);
		while (!mCallbackQueue.empty()) 
		{
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

