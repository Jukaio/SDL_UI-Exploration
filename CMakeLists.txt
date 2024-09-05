cmake_minimum_required(VERSION 3.16)
project(Testbed)

if (APPLE)
    enable_language(OBJC)
endif ()
include(FetchContent)

# Set your preferred SDL version (adjust the version numbers accordingly)
set(SDL_VERSION "main")
set(MICROUI_VERSION "master")
set(NUKLEAR_VERSION "master")

set(SDL3TTF_VENDORED ON)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
#set(WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")


# Fetch SDL3 from the Git repository
FetchContent_Declare(
        SDL
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG ${SDL_VERSION}
)

# Fetch SDL3_image
FetchContent_Declare(
        SDL_image
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
        GIT_TAG ${SDL_VERSION}
)

# Fetch SDL3_ttf
FetchContent_Declare(
        SDL_ttf
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
        GIT_TAG ${SDL_VERSION}
)

# Fetch SDL3_net
FetchContent_Declare(
        SDL_net
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_net.git
        GIT_TAG ${SDL_VERSION}
)

FetchContent_MakeAvailable(SDL SDL_image SDL_net SDL_ttf)

find_package(Freetype REQUIRED)

message(STATUS "Set sources")

file(GLOB_RECURSE FOUND_SOURCES "src/*.cpp")
list(APPEND SOURCES "${FOUND_SOURCES}")


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(${PROJECT_NAME} ${SOURCES})

include_directories("${CMAKE_SOURCE_DIR}/include")

target_link_libraries(${PROJECT_NAME} PRIVATE Freetype::Freetype SDL3::SDL3 SDL3_image::SDL3_image SDL3_net::SDL3_net SDL3_ttf::SDL3_ttf)