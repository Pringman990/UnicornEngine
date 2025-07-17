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

	template<typename T>
	void AddRaw(T* Owner, void (T::* Method)(Args...))
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

template<typename... Args>
class MultiNotifierBoolRetArgs
{
public:
	using FuncType = Func<bool(Args...)>;

	void AddLambda(FuncType Lambda)
	{
		mBoundFunctions.push_back(Lambda);
	}

	template<typename T>
	void AddRaw(T* Owner, bool (T::* Method)(Args...))
	{
		mBoundFunctions.push_back([=](Args... args) 
			{
				return (Owner->*Method)(args...); 
			});
	}

	void RemoveAll()
	{
		mBoundFunctions.clear();
	}

	bool Notify(Args... args) const
	{
		bool handled = false;
		for (const auto& func : mBoundFunctions)
		{
			//Returns true if any of the listeners return true
			if (func && func(args...))
			{
				handled = true;
			}
		}

		return handled;
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

	template<typename T>
	void AddRaw(T* Owner, void (T::* Method)())
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