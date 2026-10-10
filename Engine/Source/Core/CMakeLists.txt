# Core

add_library(Core STATIC)

file(GLOB_RECURSE CORE_SOURCES
        CONFIGURE_DEPENDS
        *.cpp
)

target_compile_definitions(Core PUBLIC
    "BUILD_EDITOR=$<BOOL:${BUILD_EDITOR}>"
)

target_sources(Core PRIVATE
        ${CORE_SOURCES}
)

target_include_directories(Core PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Core PUBLIC
)
