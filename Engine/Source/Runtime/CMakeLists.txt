# Runtime

add_library(Runtime STATIC)

file(GLOB_RECURSE RUNTIME_SOURCES
        CONFIGURE_DEPENDS
        *.cpp
)

target_sources(Runtime PRIVATE
        ${RUNTIME_SOURCES}
)

target_include_directories(Runtime PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Runtime PUBLIC
        stb
        glad
        OpenGL::GL
        SDL3::SDL3-static
        fmt::fmt-header-only
        glm::glm-header-only
)

target_precompile_headers(Runtime PRIVATE pch.h)
