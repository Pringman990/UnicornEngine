#ifdef VULKAN
    #define LOCATION(n) [[vk::location(n)]]
    #define BIND(b, s) [[vk::binding(b, s)]]
#else
    #define LOCATION(n)
    #define BIND(b, s)
#endif