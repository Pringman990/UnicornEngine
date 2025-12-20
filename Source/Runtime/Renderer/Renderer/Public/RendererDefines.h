#pragma once
#ifdef _WIN32

#ifdef RENDERER_EXPORTS
#define RENDERER_API __declspec(dllexport)
#else // RENDERER_EXPORTS
#define RENDERER_API __declspec(dllimport)
#endif // RENDERER_EXPORTS

#else // _WIN32
#define RENDERER_API 
#endif // _WIN32