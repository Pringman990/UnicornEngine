#include "EnginePch.h"
#include "StringConvert.h"

std::wstring StringToWString(const std::string& str) {
    // Get the required size for the output wide string
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);

    // Create a buffer to hold the wide string
    std::wstring wstr(size_needed, 0);

    // Perform the conversion
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size_needed);

    return wstr;
}

std::string WStringToString(const std::wstring& wstr) {
    // Get the required size for the output narrow string
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);

    // Create a buffer to hold the narrow string
    std::string str(size_needed, 0);

    // Perform the conversion
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], size_needed, NULL, NULL);

    return str;
}