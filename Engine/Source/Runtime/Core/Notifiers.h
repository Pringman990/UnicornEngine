#pragma once

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

	void AddLambda(FuncType lambda)
	{
		mSlots.push_back({ nullptr, std::move(lambda) });
	}

	template<typename T>
	void AddRaw(T* owner, void (T::* method)(Args...))
	{
		mSlots.push_back({owner, [=](Args... args) {(owner->*method)(args...); } });
	}

	void RemoveOwned(Owner owner)
	{
		mSlots.erase(
			std::remove_if(
				mSlots.begin(), mSlots.end(),
				[&](const Slot& s) {return s.owner == owner;}),
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
	List<Slot> mSlots;
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

	void AddLambda(FuncType lambda)
	{
		mSlots.push_back({ nullptr, std::move(lambda) });
	}

	template<typename T>
	void AddRaw(T* owner, bool (T::* method)(Args...))
	{
		mSlots.push_back({owner,
			[=](Args... args)
			{
				return (owner->*method)(args...);
			}
		}
		);
	}

	void RemoveOwned(Owner owner)
	{
		mSlots.erase(
			std::remove_if(
				mSlots.begin(), mSlots.end(),
				[&](const Slot& s) {return s.owner == owner;}),
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
	List<Slot> mSlots;
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

	void AddLambda(FuncType lambda)
	{
		mSlots.push_back({ nullptr, std::move(lambda) });
	}

	template<typename T>
	void AddRaw(T* owner, void (T::* method)())
	{
		mSlots.push_back({ owner, [=]() {(owner->*method)(); } });
	}

	void RemoveOwned(Owner owner)
	{
		mSlots.erase(
			std::remove_if(
				mSlots.begin(), mSlots.end(),
				[&](const Slot& s) {return s.owner == owner;}),
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
	List<Slot> mSlots;
};