cmake_minimum_required(VERSION 4.2)
project(UnicornEngine)

set(CMAKE_CXX_STANDARD 23)

find_package(OpenGL REQUIRED)

set(SDL_SHARED OFF)
set(SDL_STATIC ON)
add_subdirectory(Source/External/SDL3)

add_library(glad STATIC Source/External/glad/src/glad.c)

target_include_directories(glad PUBLIC Source/External/glad/include)

add_executable(UnicornEngine Source/Game/main.cpp
        Source/Game/Renderer.cpp
        Source/Game/Renderer.h)

target_link_libraries(UnicornEngine PRIVATE
        glad
        OpenGL::GL
        SDL3::SDL3-static
)
