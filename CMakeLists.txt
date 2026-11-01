cmake_minimum_required(VERSION 3.10)

project(microcosm C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# ---- SDL3 (path from local.cmake, which is git-ignored) ----
include("${CMAKE_CURRENT_SOURCE_DIR}/local.cmake" OPTIONAL)
find_package(SDL3 3.2 REQUIRED CONFIG)
message(STATUS "SDL3 config:  ${SDL3_DIR}")

add_executable(${PROJECT_NAME} 
    src/main.c
    src/agent.c
    src/arena.c
    src/view.c
    src/world.c
    src/pcg.c
)

add_library(project_warnings INTERFACE)
target_compile_options(project_warnings INTERFACE
    -Wall -Wextra -Wpedantic
    -Wconversion -Wsign-conversion
    -Werror
    -Wshadow

    -Wno-error=deprecated-declarations
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    SDL3::SDL3
    project_warnings
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3::SDL3>
        $<TARGET_FILE_DIR:microcosm>
    COMMENT "Copying SDL3.dll"
)

if(MINGW)
    target_link_options(microcosm PRIVATE -static-libgcc -static-libstdc++)
endif()
