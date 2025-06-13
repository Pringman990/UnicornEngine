#ifdef VULKAN
    #define LOCATION(n) [[vk::location(n)]]
#else
    #define LOCATION(n)
#endif