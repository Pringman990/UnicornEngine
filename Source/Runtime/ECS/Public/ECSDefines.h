#pragma once
#ifdef _WIN32

#ifdef ECS_EXPORTS
#define ECS_API __declspec(dllexport)
#else // ECS_EXPORTS
#define ECS_API __declspec(dllimport)
#endif // ECS_EXPORTS

#else // _WIN32
#define ECS_API 
#endif // _WIN32