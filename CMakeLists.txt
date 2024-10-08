cmake_minimum_required(VERSION 3.11) # FetchContent is available in 3.11+
project(zoctante
    LANGUAGES C
    DESCRIPTION "Space invaders emulator"
)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies
set(RAYLIB_VERSION 5.0)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
if(NOT raylib_FOUND) # If there's none, fetch and build raylib
    include(FetchContent)
    FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
  )
    FetchContent_GetProperties(raylib)
    if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
        set(FETCHCONTENT_QUIET NO)
        FetchContent_MakeAvailable(raylib)
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
        set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # or games
        # add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
    endif()
endif()

# Our Project

# zoctante
set(SOURCES
    src/main.c
    src/8080.c
    src/machine.c
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC src)

# set(raylib_VERBOSE 1)
target_link_libraries(${PROJECT_NAME} raylib)

add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND}
    ARGS -E rm $<TARGET_FILE:${PROJECT_NAME}>
)

# test
set(SOURCES
    src/test.c
    src/8080.c
    src/test_machine.c
)

add_executable(test ${SOURCES})
target_include_directories(test PUBLIC src)
target_link_libraries(test raylib)

add_custom_command(
    TARGET test
    POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy $<TARGET_FILE:test> ${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND}
    ARGS -E rm $<TARGET_FILE:test>
)

# example

# set(SOURCES
#     ./raygui-4.0/examples/floating_window/floating_window.c
# )
#
# add_executable(example ${SOURCES})
# target_include_directories(example PUBLIC ./raygui-4.0/examples/)
# target_link_libraries(example raylib)
#
# add_custom_command(
#     TARGET example
#     POST_BUILD
#     COMMAND ${CMAKE_COMMAND}
#     ARGS -E copy $<TARGET_FILE:example> ${CMAKE_SOURCE_DIR}
#     COMMAND ${CMAKE_COMMAND}
#     ARGS -E rm $<TARGET_FILE:example>
# )


# Web Configurations
if(${PLATFORM} STREQUAL "Web")
    # Tell Emscripten to build an example.html file.
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
endif()

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if(APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
