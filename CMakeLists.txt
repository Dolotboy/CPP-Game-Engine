cmake_minimum_required(VERSION 3.16)

project(GameEngine
    VERSION 1.0
    LANGUAGES CXX
)

# --------------------------------------------------
# Configuration C++
# --------------------------------------------------

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --------------------------------------------------
# SFML
# --------------------------------------------------

find_package(SFML 2.6 COMPONENTS
    graphics
    window
    system
    REQUIRED
)

# --------------------------------------------------
# Sources
# --------------------------------------------------

file(GLOB_RECURSE GAME_ENGINE_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/GameEngine/Core/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/GameEngine/Game/*.cpp"
)

# --------------------------------------------------
# Executable
# --------------------------------------------------

add_executable(GameEngine
    ${GAME_ENGINE_SOURCES}
)

# --------------------------------------------------
# Include directories
# --------------------------------------------------

target_include_directories(GameEngine PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/GameEngine"
)

# --------------------------------------------------
# Compiler warnings
# --------------------------------------------------

if(MSVC)
    target_compile_options(GameEngine PRIVATE
        /W4
    )
else()
    target_compile_options(GameEngine PRIVATE
        -Wall
        -Wextra
        -pedantic
    )
endif()

# --------------------------------------------------
# SFML linking
# --------------------------------------------------

target_link_libraries(GameEngine PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
)

# --------------------------------------------------
# Output directories
# --------------------------------------------------

set_target_properties(GameEngine PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# --------------------------------------------------
# SFML DLLs - Windows
# --------------------------------------------------

if(WIN32)
    add_custom_command(TARGET GameEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${SFML_DIR}/../../../bin"
            "$<TARGET_FILE_DIR:GameEngine>"
    )
endif()
