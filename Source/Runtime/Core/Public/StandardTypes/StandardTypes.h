#pragma once
#include <stdint.h>
#include <memory>
#include <vector>
#include <string>

using uint8	 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

using int8	= int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;

using wchar = wchar_t;

using byte = uint8;
using ByteBuffer = std::vector<byte>;

using String = std::string;

template<typename T>
using SharedPtr = std::shared_ptr<T>;

template<typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename T>
using OwnedPtr = std::unique_ptr<T>;

template<typename T, typename... Args>
OwnedPtr<T> MakeOwned(Args&&... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}
