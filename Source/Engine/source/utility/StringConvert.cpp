#include "EnginePch.h"
#include "StringConvert.h"

#include <locale>
#include <codecvt>

std::wstring StringToWString(const std::string& str) 
{
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);

    std::wstring wstr(size_needed, 0);

    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size_needed);
    wstr.resize(size_needed - 1); //Remove null teminator
    return wstr;
}

std::string WStringToString(const std::wstring& wstr) 
{
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);

    std::string str(size_needed, 0);

    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], size_needed, NULL, NULL);
    str.resize(size_needed - 1); //Remove null teminator
    return str;
}