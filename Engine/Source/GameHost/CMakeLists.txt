# GameHost

add_executable(GameHost)

target_sources(GameHost PRIVATE
        main.cpp
)

target_link_libraries(GameHost PRIVATE
        Sandbox
)

if (WIN32)
    set_target_properties(GameHost PROPERTIES
            WIN32_EXECUTABLE $<CONFIG:Release>
    )
endif()

set_target_properties(GameHost PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG
        "${CMAKE_SOURCE_DIR}/Build/Debug"

        RUNTIME_OUTPUT_DIRECTORY_RELEASE
        "${CMAKE_SOURCE_DIR}/Build/Release"
)

add_custom_command(
        TARGET GameHost POST_BUILD

        COMMAND ${CMAKE_COMMAND}
        -E copy_directory

        ${CMAKE_SOURCE_DIR}/Sandbox/Assets
        $<TARGET_FILE_DIR:GameHost>/Assets
)