#pragma once

template<typename _Type, size_t _Alignment>
class AlignedAllocator
{
public:
	using value_type = _Type;

	AlignedAllocator() noexcept = default;

	//template <typename U>
	//struct rebind {
	//	using other = AlignedAllocator<U, _Alignment>;
	//};

	//_Type* allocate(std::size_t NumberToAllocate)
	//{
	//	void* ptr = ::operator new(NumberToAllocate, std::align_val_t(_Alignment));
	//	if (!ptr) {
	//		throw std::bad_alloc();
	//	}
	//	return static_cast<_Type*>(ptr);
	//}

	//void deallocate(void* Data, std::size_t)
	//{
	//	::operator delete(Data, std::align_val_t(_Alignment));
	//}

private:
};

