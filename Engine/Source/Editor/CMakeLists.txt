# Editor

add_library(Editor STATIC)

file(GLOB_RECURSE EDITOR_SOURCES
        CONFIGURE_DEPENDS
        *.cpp
)

target_sources(Editor PRIVATE
        ${EDITOR_SOURCES}
)

target_include_directories(Editor PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Editor PUBLIC
        Runtime
        imgui
)

target_precompile_headers(Editor PRIVATE pch.h)