# Sandbox

add_library(Sandbox STATIC)

file(GLOB_RECURSE SANDBOX_SOURCES
        CONFIGURE_DEPENDS
        Src/*.cpp
)

target_sources(Sandbox PRIVATE
        ${SANDBOX_SOURCES}
)

target_link_libraries(Sandbox PUBLIC
        Runtime
)

target_include_directories(Sandbox PUBLIC
        Src
)

target_precompile_headers(Sandbox PRIVATE
        Src/pch.h
)