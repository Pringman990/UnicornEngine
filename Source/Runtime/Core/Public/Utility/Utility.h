#pragma once
#include <string>
#include <random>
#include <SimpleMath.h>
#include "StandardTypes/StandardTypes.h"

/// <summary>
 /// Adds element to the back of the vector
 /// </summary>
 /// <returns>
 /// TRUE if value was not already in the vector.
 /// FALSE if value already exists
 /// </returns>
template<typename T>
inline bool AddUniqueToVector(std::vector<T>& aVector, T& aItem)
{
    auto it = std::find(aVector.begin(), aVector.end(), aItem);
    if (it == aVector.end())
    {
        aVector.push_back(aItem);
        return true;
    }
    return false;
}

/// <summary>
 /// Removes item from a vector
 /// </summary>
 /// <returns>
 /// TRUE if item was found and removed
 /// FALSE if item was not found in the vector
 /// </returns>
template<typename T>
inline bool EraseItemFromVector(std::vector<T>& aVector, T& aItem)
{
    auto it = std::find(aVector.begin(), aVector.end(), aItem);
    if (it != aVector.end())
    {
        aVector.erase(it);
        return true;
    }
    return false;
}

inline std::string WStringToString(const std::wstring& aWideString)
{
    size_t size_needed = 0;
    // Get the required size
    wcstombs_s(&size_needed, nullptr, 0, aWideString.c_str(), 0);
    if (size_needed == 0) throw std::runtime_error("Conversion failed");

    std::string basicString;
    basicString.resize(size_needed);

    // Convert the string
    wcstombs_s(&size_needed, &basicString[0], basicString.size() + 1, aWideString.c_str(), aWideString.size() + 1);

    if (!basicString.empty() && basicString.back() == '\0') {
        basicString.pop_back();
    }

    return basicString;
}

inline std::wstring StringToWString(const std::string& aString)
{
    size_t size_needed = 0;
    // Get the required size
    mbstowcs_s(&size_needed, nullptr, 0, aString.c_str(), 0);
    if (size_needed == 0) throw std::runtime_error("Conversion failed");

    std::wstring wideString;
    wideString.resize(size_needed);
    // Convert the string
    mbstowcs_s(&size_needed, &wideString[0], wideString.size() + 1, aString.c_str(), aString.size() + 1);

    if (!wideString.empty() && wideString.back() == '\0') {
        wideString.pop_back();
    }

    return wideString;
}

inline int32 RandomInt(int32 aMin, int32 aMax)
{
    static std::random_device rd;  // Seed
    static std::mt19937 rng(rd()); // Random number generator
    std::uniform_int_distribution<int32> dist(aMin, aMax);
    return dist(rng);
}

inline float RandomFloat(float aMin, float aMax)
{
    static std::random_device rd;  // Seed
    static std::mt19937 rng(rd()); // Random number generator
    std::uniform_real_distribution<float> dist(aMin, aMax);
    return dist(rng);
}

inline Color OffsetColor(Color VectorToModify, float RModifier, float GModifier, float BModifier)
{
    Color resultVector;

    resultVector.x = VectorToModify.x * RandomFloat(1 - RModifier, 1 + RModifier);
    resultVector.y = VectorToModify.y * RandomFloat(1 - GModifier, 1 + GModifier);
    resultVector.z = VectorToModify.z * RandomFloat(1 - BModifier, 1 + BModifier);
    resultVector.w = VectorToModify.w;
    return resultVector;
}

inline Color OffsetColor(Color VectorToModify, float Modifier)
{
    return OffsetColor(VectorToModify, Modifier, Modifier, Modifier);
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