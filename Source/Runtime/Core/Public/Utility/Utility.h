#pragma once
#include <string>
#include <random>
#include <SimpleMath.h>
#include "StandardTypes/StandardTypes.h"

 /**
 * Adds element to the back of the vector.
 * 
 * @return If value was not already in the vector.
 * 
 * @ingroup global
 */
template<typename T>
inline bool AddUniqueToVector(Vector<T>& Vector, T& Item)
{
    auto it = std::find(Vector.begin(), Vector.end(), Item);
    if (it == Vector.end())
    {
        Vector.push_back(Item);
        return true;
    }
    return false;
}

/**
* Removes item from a vector.
*
* @return If item was found and removed.
*
* @ingroup global
*/
template<typename T>
inline bool EraseItemFromVector(Vector<T>& Vector, T& Item)
{
    auto it = std::find(Vector.begin(), Vector.end(), Item);
    if (it != Vector.end())
    {
        Vector.erase(it);
        return true;
    }
    return false;
}

/**
* Converts a std::wstring to std::string.
*
* @ingroup global
*/
inline String WStringToString(const std::wstring& WideString)
{
    size_t size_needed = 0;
    // Get the required size
    wcstombs_s(&size_needed, nullptr, 0, WideString.c_str(), 0);
    if (size_needed == 0) throw std::runtime_error("Conversion failed");

    String basicString;
    basicString.resize(size_needed);

    // Convert the string
    wcstombs_s(&size_needed, &basicString[0], basicString.size() + 1, WideString.c_str(), WideString.size() + 1);

    if (!basicString.empty() && basicString.back() == '\0') {
        basicString.pop_back();
    }

    return basicString;
}

/**
* Converts a std::string to std::wstring.
*
* @ingroup global
*/
inline std::wstring StringToWString(const String& String)
{
    size_t size_needed = 0;
    // Get the required size
    mbstowcs_s(&size_needed, nullptr, 0, String.c_str(), 0);
    if (size_needed == 0) throw std::runtime_error("Conversion failed");

    std::wstring wideString;
    wideString.resize(size_needed);
    // Convert the string
    mbstowcs_s(&size_needed, &wideString[0], wideString.size() + 1, String.c_str(), String.size() + 1);

    if (!wideString.empty() && wideString.back() == '\0') {
        wideString.pop_back();
    }

    return wideString;
}

/**
* Gives a random int32 between min and max.
*
* @ingroup global
*/
inline int32 RandomInt(int32 Min, int32 Max)
{
    static std::random_device rd;  // Seed
    static std::mt19937 rng(rd()); // Random number generator
    std::uniform_int_distribution<int32> dist(Min, Max);
    return dist(rng);
}

/**
* Gives a random float between min and max.
*
* @ingroup global
*/
inline float RandomFloat(float Min, float Max)
{
    static std::random_device rd;  // Seed
    static std::mt19937 rng(rd()); // Random number generator
    std::uniform_real_distribution<float> dist(Min, Max);
    return dist(rng);
}

template <typename T>
inline void hash_combine(size_t& seed, const T& val) 
{
    seed ^= std::hash<T>()(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

inline size_t HashMemory(const void* data, size_t size) 
{
    std::string_view view(reinterpret_cast<const char*>(data), size);
    return std::hash<std::string_view>{}(view);
}