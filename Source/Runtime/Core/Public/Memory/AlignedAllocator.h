#pragma once
#include <Core.h>

template<typename _Type, size_t _Alignment>
class AlignedAllocator
{
public:
	using value_type = _Type;

	AlignedAllocator() noexcept = default;

	template <typename U>
	struct rebind {
		using other = AlignedAllocator<U, _Alignment>;
	};

	_Type* allocate(std::size_t aNumberToAllocate)
	{
		void* ptr = ::operator new(aNumberToAllocate, std::align_val_t(_Alignment));
		if (!ptr) {
			throw std::bad_alloc();
		}
		return static_cast<_Type*>(ptr);
	}

	void deallocate(void* someData, std::size_t)
	{
		::operator delete(someData, std::align_val_t(_Alignment));
	}

private:
};

