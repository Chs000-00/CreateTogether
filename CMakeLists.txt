cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(CreateTogether VERSION 0.2.0)

file(GLOB SOURCES
    src/hooks/*.cpp
    src/hooks/EditorOptions/*.cpp
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
set(USE_TEST_SERVER ON)
add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

# Thankye globed
# Can *Safely* be commented out when not compiling with test server
if (WIN32 AND USE_TEST_SERVER)
    message(STATUS "Compiling with test server")

    # geode moment
    # Enable experimental support for a server at 24018, '127.0.0.1'
    # Incase you dont have friends and need to test this app out
    add_compile_definitions(
        WIN32_LEAN_AND_MEAN=1
        USE_TEST_SERVER=1
    ) 
    target_link_libraries(${PROJECT_NAME} ws2_32)
endif()


target_include_directories(${PROJECT_NAME} PUBLIC include)

# Set up dependencies, resources, and link Geode.
setup_geode_mod(${PROJECT_NAME})

CPMAddPackage("gh:crashoz/uuid_v4#41238a4")

target_link_libraries(${PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/steam_api64.lib
    uuid_v4::uuid_v4
)
