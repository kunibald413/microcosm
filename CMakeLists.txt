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

add_executable(microcosm 
    src/main.c
    src/agent.c
    src/view.c
)

target_link_libraries(microcosm PRIVATE SDL3::SDL3)

add_custom_command(TARGET microcosm POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3::SDL3>
        $<TARGET_FILE_DIR:microcosm>
    COMMENT "Copying SDL3.dll"
)

if(MINGW)
    target_link_options(microcosm PRIVATE -static-libgcc -static-libstdc++)
endif()
