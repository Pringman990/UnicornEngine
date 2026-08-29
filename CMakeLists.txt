# Root

cmake_minimum_required(VERSION 4.2)
project(UnicornEngine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ThirdParty dependencies
find_package(OpenGL REQUIRED)

set(SDL_SHARED OFF)
set(SDL_STATIC ON)

add_subdirectory(ThirdParty/SDL3)

add_library(glad STATIC
        ThirdParty/glad/src/glad.c
)

target_include_directories(glad PUBLIC
        ThirdParty/glad/include
)

add_library(stb INTERFACE)

target_include_directories(stb INTERFACE
        ThirdParty/stb
)

add_subdirectory(ThirdParty/glm)

set(FMT_MODULE OFF CACHE BOOL "" FORCE)
add_subdirectory(ThirdParty/fmt EXCLUDE_FROM_ALL)

add_subdirectory(Engine/Source/Runtime)
add_subdirectory(Sandbox)
add_subdirectory(Engine/Source/GameHost)
