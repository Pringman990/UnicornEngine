#pragma once
#include <functional>
#include <StandardTypes/StandardTypes.h>

template<typename... Args>
class MultiNotifierArgs
{
public:
	using FuncType = Func<void(Args...)>;
	using Owner = void*;
private:
	struct Slot
	{
		Owner owner;
		FuncType func;
	};
public:

	void AddLambda(FuncType Lambda)
	{
		mSlots.push_back({ nullptr, std::move(Lambda) });
	}

	template<typename T>
	void AddRaw(T* Owner, void (T::* Method)(Args...))
	{
		mSlots.push_back({Owner, [=](Args... args) {(Owner->*Method)(args...); } });
	}

	void RemoveOwned(Owner Owner)
	{
		mSlots.erase(
			std::remove_if(
				mSlots.begin(), mSlots.end(),
				[&](const Slot& s) {return s.owner == Owner;}), 
				mSlots.end()
		);
	}

	void RemoveAll()
	{
		mSlots.clear();
	}

	void Notify(Args... args) const
	{
		for (auto& slot : mSlots)
		{
			if (slot.func)
			{
				slot.func(args...);
			}
		}
	}

private:
	Vector<Slot> mSlots;
};

template<typename... Args>
class MultiNotifierBoolRetArgs
{
public:
	using FuncType = Func<bool(Args...)>;
	using Owner = void*;
private:
	struct Slot
	{
		Owner owner;
		FuncType func;
	};
public:	

	void AddLambda(FuncType Lambda)
	{
		mSlots.push_back({ nullptr, std::move(Lambda) });
	}

	template<typename T>
	void AddRaw(T* Owner, bool (T::* Method)(Args...))
	{
		mSlots.push_back({Owner, 
			[=](Args... args)
			{
				return (Owner->*Method)(args...);
			} 
		}
		);
	}

	void RemoveOwned(Owner Owner)
	{
		mSlots.erase(
			std::remove_if(
				mSlots.begin(), mSlots.end(),
				[&](const Slot& s) {return s.owner == Owner;}),
			mSlots.end()
		);
	}

	void RemoveAll()
	{
		mSlots.clear();
	}

	bool Notify(Args... args) const
	{
		bool handled = false;
		for (auto& slot : mSlots)
		{
			//Returns true if any of the listeners return true
			if (slot.func && slot.func(args...))
			{
				handled = true;
			}
		}

		return handled;
	}

private:
	Vector<Slot> mSlots;
};

class MultiNotifier
{
public:
	using FuncType = Func<void()>;
	using Owner = void*;
private:
	struct Slot
	{
		Owner owner;
		FuncType func;
	};
public:
	
	void AddLambda(FuncType Lambda)
	{
		mSlots.push_back({ nullptr, std::move(Lambda) });
	}

	template<typename T>
	void AddRaw(T* Owner, void (T::* Method)())
	{
		mSlots.push_back({ Owner, [=]() {(Owner->*Method)(); } });
	}

	void RemoveOwned(Owner Owner)
	{
		mSlots.erase(
			std::remove_if(
				mSlots.begin(), mSlots.end(),
				[&](const Slot& s) {return s.owner == Owner;}),
			mSlots.end()
		);
	}

	void RemoveAll()
	{
		mSlots.clear();
	}

	void Notify()
	{
		for (auto& slot : mSlots)
		{
			if (slot.func)
			{
				slot.func();
			}
		}
	}

private:
	Vector<Slot> mSlots;
};