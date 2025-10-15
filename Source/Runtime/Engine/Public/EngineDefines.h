#ifdef _WIN32

#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else // ENGINE_EXPORTS
#define ENGINE_API __declspec(dllimport)
#endif // ENGINE_EXPORTS

#else // _WIN32
#define ENGINE_API 
#endif // _WIN32

#define TRUNCATE_IMPL(a, b) a##b
#define TRUNCATE(a, b) TRUNCATE_IMPL(a, b)