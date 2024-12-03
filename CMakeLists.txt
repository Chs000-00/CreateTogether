cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(CreateTogether VERSION 0.1.1)

add_compile_definitions(
    MOD_LOBBY_NAME="${PROJECT_NAME}"
    MOD_VERSION="${PROJECT_VERSION}"
)


file(GLOB SOURCES
    src/hooks/*.cpp
    src/layers/*.cpp
    src/ui/*.cpp
    src/types/*.cpp
    src/utils/*.cpp
)
# Set up the mod binary
add_library(${PROJECT_NAME} SHARED ${SOURCES})

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

set(GEODE_LINK_NIGHTLY ON)
add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

target_include_directories(${PROJECT_NAME} PUBLIC include)

target_link_libraries(${PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/steam_api64.lib
)

# Set up dependencies, resources, and link Geode.
setup_geode_mod(${PROJECT_NAME})

