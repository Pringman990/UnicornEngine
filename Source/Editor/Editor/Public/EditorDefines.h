#pragma once
#ifdef _WIN32

#ifdef EDITOR_EXPORTS
#define EDITOR_API __declspec(dllexport)
#else // EDITOR_EXPORTS
#define EDITOR_API __declspec(dllimport)
#endif // EDITOR_EXPORTS

#else // _WIN32
#define EDITOR_API 
#endif // _WIN32