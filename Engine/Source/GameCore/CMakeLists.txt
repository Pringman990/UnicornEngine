# GameCore

add_library(GameCore STATIC)

file(GLOB_RECURSE GAME_CORE_SOURCES
        CONFIGURE_DEPENDS
        *.cpp
)

target_sources(GameCore PRIVATE
        ${GAME_CORE_SOURCES}
)

target_include_directories(GameCore PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(GameCore PUBLIC
        Runtime
)

target_precompile_headers(GameCore PRIVATE pch.h)
