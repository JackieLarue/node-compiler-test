cmake_minimum_required(VERSION 3.10)
project(compiler_test C)
set(CMAKE_C_STANDARD 99)

# Adding Raylib
include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # don't build the supplied example games
FetchContent_Declare(
    raylib 
    GIT_REPOSITORY https://github.com/raysan5/raylib.git 
    GIT_TAG master
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE    
)
FetchContent_MakeAvailable(raylib)

# Adding our source files
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/src/*.c") # Define PROJECT_SOURCES as a list of all source files
set(PROJECT_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/src/") # Define PROJECT_INCLUDE to be the path to the include directory of the project
#set(EXTERNAL_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/include/") # Define EXTERNAL_INCLUDE to be the path to the external lib include dir of the project

# Declaring our executable
add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDE})
target_link_libraries(${PROJECT_NAME} PUBLIC raylib)
#set(CMAKE_CXX_FLAGS "-lm raylib")
set(CMAKE_C_FLAGS_DEBUG "-Wall -Werror -Wextra -Wno-error=sign-compare -Wno-error=missing-braces -std=gnu99")
set(CMAKE_C_FLAGS_RELEASE "-O3 -std=gnu99")

# Setting ASSETS_PATH
#target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/") # Set the asset path macro to the absolute path on the dev machine
add_compile_definitions(ASSETS_PATH="./assets") # Set the asset path macro in release mode to a relative path that assumes the assets folder is in the same directory as the game executable
add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        ${CMAKE_CURRENT_BINARY_DIR}/assets)