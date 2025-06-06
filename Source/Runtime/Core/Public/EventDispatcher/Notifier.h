#pragma once
#include <functional>

template<typename... Args>
class MultiNotifierArgs
{
public:
	using FuncType = Func<void(Args...)>;

	void AddLambda(FuncType Lambda)
	{
		mBoundFunctions.push_back(Lambda);
	}

	template<typename Owner>
	void AddRaw(Owner* Owner, void (Owner::* Method)(Args...))
	{
		mBoundFunctions.push_back([=](Args... args) {(Owner->*Method)(args...); });
	}

	void RemoveAll()
	{
		mBoundFunctions.clear();
	}

	void Notify(Args... args) const
	{
		for (const auto& func : mBoundFunctions)
		{
			if (func)
			{
				func(args...);
			}
		}
	}

private:
	Vector<FuncType> mBoundFunctions;
};

class MultiNotifier
{
public:
	using FuncType = Func<void()>;

	void AddLambda(FuncType Lambda)
	{
		mBoundFunctions.push_back(Lambda);
	}

	template<typename Owner>
	void AddRaw(Owner* Owner, void (Owner::* Method)())
	{
		mBoundFunctions.push_back([=]() {(Owner->*Method)(); });
	}

	void RemoveAll()
	{
		mBoundFunctions.clear();
	}

	void Notify()
	{
		for (const auto& func : mBoundFunctions)
		{
			if (func)
			{
				func();
			}
		}
	}

private:
	Vector<FuncType> mBoundFunctions;
};