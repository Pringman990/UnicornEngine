#pragma once
#include <string>

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

    return wideString;
}