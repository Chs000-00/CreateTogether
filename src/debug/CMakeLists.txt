cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(server)

add_executable(${PROJECT_NAME} server.cpp)

target_link_libraries(${PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/SteamWorkless/GameNetworkingSockets.lib
)

target_include_directories(server PUBLIC ../../include)

file(GLOB NETWORKING_LIBRARIES
  "../../lib/SteamWorkless/*.dll"
)

file(COPY ${NETWORKING_LIBRARIES} DESTINATION ../../src/debug)