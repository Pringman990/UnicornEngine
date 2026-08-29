#pragma once
#include <bitset>
#include <cstdint>
#include <filesystem>
#include <string>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <optional>
#include <array>
#include <functional>
#include <variant>
#include <expected>
#include <chrono>
#include <deque>
#include <mutex>
#include <memory>
#include <span>
#include <string_view>
#include <thread>
#include <vector>

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
using StringView = std::string_view;
using Path = std::filesystem::path;

template<typename T>
using List = std::vector<T>;

template<typename T>
using Span = std::span<T>;

using ByteBuffer = List<byte>;

template<typename T, std::size_t N>
using Array = std::array<T, N>;

template<typename T, typename V>
using Map = std::map<T, V>;

template<typename T, typename V, typename H = std::hash<T>>
using UnorderedMap = std::unordered_map<T, V, H>;

template<typename T, typename V>
using MultiMap = std::multimap<T, V>;

template<typename T>
using Set = std::set<T>;

template<typename T>
using HashSet = std::unordered_set<T>;

template<size_t T>
using BitSet = std::bitset<T>;

template<typename T>
using Queue = std::queue<T>;

template<typename T>
using Deque = std::deque<T>;

template<typename T>
using Func = std::function<T>;

template<typename T>
using Optional = std::optional<T>;

template<typename... Ts>
using Variant = std::variant<Ts...>;

template<typename T1, typename T2>
using Pair = std::pair<T1, T2>;

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

template<typename T>
using WeakPtr = std::weak_ptr<T>;

template<typename T, typename E>
using Expected = std::expected<T, E>;

template<typename E>
using Unexpected = std::unexpected<E>;

using TimePoint = std::chrono::system_clock::time_point;

using ThreadID = std::thread::id;

template<typename T>
using Atomic = std::atomic<T>;

using Mutex = std::mutex;
