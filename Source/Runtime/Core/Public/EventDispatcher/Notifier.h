#pragma once

template<typename... Args>
class MultiNotifierArgs
{
public:
	using FuncType = std::function<void(Args...)>;

	void AddLambda(FuncType aLambda)
	{
		mBoundFunctions.push_back(aLambda);
	}

	template<typename Owner>
	void AddRaw(Owner* aOwner, void (Owner::* method)(Args...))
	{
		mBoundFunctions.push_back([=](Args... args) {(aOwner->*method)(args...); });
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
	std::vector<FuncType> mBoundFunctions;
};

class MultiNotifier
{
public:
	using FuncType = std::function<void()>;

	void AddLambda(FuncType aLambda)
	{
		mBoundFunctions.push_back(aLambda);
	}

	template<typename Owner>
	void AddRaw(Owner* aOwner, void (Owner::* method)())
	{
		mBoundFunctions.push_back([=]() {(aOwner->*method)(); });
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
	std::vector<FuncType> mBoundFunctions;
};