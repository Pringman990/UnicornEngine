# Game

add_library(Game STATIC)

file(GLOB_RECURSE GAME_SOURCES
        CONFIGURE_DEPENDS
        Src/*.cpp
)

target_sources(Game PRIVATE
        ${GAME_SOURCES}
)

target_link_libraries(Game PUBLIC
        GameCore
)

target_include_directories(Game PUBLIC
        Src
)

target_precompile_headers(Game PRIVATE
        Src/pch.h
)