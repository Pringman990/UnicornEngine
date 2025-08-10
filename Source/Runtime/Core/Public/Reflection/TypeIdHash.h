#pragma once

#ifdef _MSC_VER
#define FUNCTION_SIGNATURE __FUNCSIG__
#else
#define FUNCTION_SIGNATURE __PRETTY_FUNCTION__
#endif // 


using TypeIdHash = size_t;

constexpr TypeIdHash ConstexprHash(const char* Str)
{
	TypeIdHash hash = 14695981039346656037ULL;
    while (*Str) 
    {
        hash ^= static_cast<unsigned char>(*Str++);
        hash *= 1099511628211ULL;
    }
    return hash;
}

template<typename T>
constexpr TypeIdHash GetTypeId()
{
    return ConstexprHash(FUNCTION_SIGNATURE);
}