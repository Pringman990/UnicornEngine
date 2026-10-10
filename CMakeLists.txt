# Root

cmake_minimum_required(VERSION 4.2)
project(UnicornEngine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_EDITOR "Build editor tooling" ON)

add_subdirectory(Engine/Source/Core)
add_subdirectory(Engine/Source/Runtime)
add_subdirectory(Engine/Source/GameCore)
add_subdirectory(Game)
add_subdirectory(Editor)
add_subdirectory(Host)
