# Host

add_executable(Host)

target_sources(Host PRIVATE
        Host.cpp
)

target_link_libraries(Host PRIVATE
        Game
        $<$<CONFIG:Debug>:Editor>
)

if (WIN32)
    set_target_properties(Host PROPERTIES
            WIN32_EXECUTABLE $<CONFIG:Release>
    )
endif()

set_target_properties(Host PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG
        "${CMAKE_SOURCE_DIR}/Build/Debug"

        RUNTIME_OUTPUT_DIRECTORY_RELEASE
        "${CMAKE_SOURCE_DIR}/Build/Release"
)

add_custom_command(
        TARGET Host POST_BUILD

        COMMAND ${CMAKE_COMMAND}
        -E copy_directory

        ${CMAKE_SOURCE_DIR}/Game/Assets
        $<TARGET_FILE_DIR:Host>/Assets
)