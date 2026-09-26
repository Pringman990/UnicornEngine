# Root

cmake_minimum_required(VERSION 4.2)
project(UnicornEngine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# ThirdParty dependencies
find_package(OpenGL REQUIRED)

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)

# Linux window backends
if(UNIX AND NOT APPLE)
    set(SDL_WAYLAND ON CACHE BOOL "" FORCE)
    set(SDL_X11 ON CACHE BOOL "" FORCE)
endif()

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

add_library(imgui STATIC
        ${CMAKE_SOURCE_DIR}/ThirdParty/imgui/backends/imgui_impl_sdl3.cpp
        ${CMAKE_SOURCE_DIR}/ThirdParty/imgui/backends/imgui_impl_opengl3.cpp
        ${CMAKE_SOURCE_DIR}/ThirdParty/imgui/imgui.cpp
        ${CMAKE_SOURCE_DIR}/ThirdParty/imgui/imgui_draw.cpp
        ${CMAKE_SOURCE_DIR}/ThirdParty/imgui/imgui_tables.cpp
        ${CMAKE_SOURCE_DIR}/ThirdParty/imgui/imgui_widgets.cpp
        ${CMAKE_SOURCE_DIR}/ThirdParty/imgui/misc/cpp/imgui_stdlib.cpp
)

target_include_directories(imgui PUBLIC
        ${CMAKE_SOURCE_DIR}/ThirdParty/imgui
        ${CMAKE_SOURCE_DIR}/ThirdParty/SDL3/include
)

set(FMT_MODULE OFF CACHE BOOL "" FORCE)
add_subdirectory(ThirdParty/fmt EXCLUDE_FROM_ALL)

add_subdirectory(ThirdParty/yaml-cpp)

add_subdirectory(Engine/Source/Runtime)
add_subdirectory(Sandbox)
add_subdirectory(Engine/Source/Editor)
add_subdirectory(Engine/Source/GameHost)
