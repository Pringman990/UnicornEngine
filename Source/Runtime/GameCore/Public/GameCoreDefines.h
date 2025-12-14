#pragma once
#ifdef _WIN32

#ifdef GAMECORE_EXPORTS
#define GAMECORE_API __declspec(dllexport)
#else // GAMECORE_EXPORTS
#define GAMECORE_API __declspec(dllimport)
#endif // GAMECORE_EXPORTS

#else // _WIN32
#define GAMECORE_API 
#endif // _WIN32