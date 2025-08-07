cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "iOS" OR IOS)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
else()
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
endif()

set(CMAKE_CXX_VISIBILITY_PRESET hidden)

set(NO_STEAMWORKS OFF)

# This is different from NO_STEAMWORKS.
set(COMPILE_DEBUG_SERVER OFF)

if (COMPILE_DEBUG_SERVER)
    message(status "Compiling debug server")

    # project(CTDebugServer VERSION 1.0.0)

    file(GLOB SERVER_SOURCES
        src/debug/server.cpp
    )

    add_executable(server ${SERVER_SOURCES})

    target_link_libraries(server
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/SteamWorkless/GameNetworkingSockets.lib
    )

    target_include_directories(server PUBLIC include)

    string( REPLACE "/DNDEBUG" "" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")

endif()

project(CreateTogether VERSION 0.5.1)

file(GLOB SOURCES
    src/hooks/*.cpp
    src/hooks/PlayLayer/*.cpp
    src/hooks/EditorOptions/*.cpp
    src/layers/*.cpp
    src/ui/*.cpp
    src/types/*.cpp
    src/utils/*.cpp
    src/networking/*.cpp
    src/networking/highLevel/*.cpp
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

# Thankye globed
# Can *Safely* be commented out when not compiling with test server
if (NO_STEAMWORKS)
    message(STATUS "Compiling without steamworks")


    # Enable experimental support for a server at port 24018, address '127.0.0.1'
    # Incase you dont have friend's and need to test this mod out
    # A button should appear in your level list. One of them joins the server as host, the friends list joins the server as not-host
    
    # WARNING! Compiling this requires you to install GameNetworkingSockets and all
    # of it's stuff into ./lib . If you want my lib binaries feel free to DM me or something idk.

    target_link_libraries(${PROJECT_NAME}
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/SteamWorkless/GameNetworkingSockets.lib
    )


    add_compile_definitions(
        NO_STEAMWORKS=1
    ) 

else()

    message(STATUS "Compiling with steamworks")

    if (ANDROID OR IOS)
        message(WARNING "IOS / Android has no support for steamworks. Please disable NO_STEAMWORKS.")
    endif()

    target_link_libraries(${PROJECT_NAME}
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/steam_api64.lib
    )

    # Small change to make code more readable, as it could get easy to mix ifndef NO_STEAMWORKS and ifdef NO_STEAMWORKS
    add_compile_definitions(
        STEAMWORKS=1
    )

endif()


target_include_directories(${PROJECT_NAME}
    PUBLIC include
    PUBLIC src/networking
)


# Set up dependencies, resources, and link Geode.
setup_geode_mod(${PROJECT_NAME})

# UUIDv4 was removed a long time ago, and is no longer used.
# CPMAddPackage("gh:crashoz/uuid_v4#41238a4")
