#pragma once
#include <stdint.h>
#include <memory>
#include <vector>
#include <array>
#include <string>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <functional>
#include <queue>
#include <optional>
#include <span>
#include <typeindex>
#include <mutex>
#include <variant>

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

using String = std::string;

template<typename T>
using Vector = std::vector<T>;

using ByteBuffer = Vector<byte>;

template<typename T, std::size_t N>
using Array = std::array<T, N>;

template<typename T, typename V>
using Map = std::map<T, V>;

template<typename T, typename V>
using UnorderedMap = std::unordered_map<T, V>;

template<typename T, typename V>
using MultiMap = std::multimap<T, V>;

template<typename T>
using Set = std::set<T>;

template<typename T>
using UnorderedSet = std::unordered_set<T>;

template<typename T>
using Queue = std::queue<T>;

template<typename T>
using Deque = std::deque<T>;

template<typename T>
using Func = std::function<T>;

template<typename T>
using SharedPtr = std::shared_ptr<T>;

template<typename T>
using Optional = std::optional<T>;

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

template<typename>
inline constexpr bool dependent_false_v = false;

using ConstructFunc = void(*)(void* Dst);
using CopyFunc = void(*)(void* Dst, const void* Src);
using MoveFunc = void(*)(void* Dst, void* Src);
using DestroyFunc = void(*)(void* Obj);