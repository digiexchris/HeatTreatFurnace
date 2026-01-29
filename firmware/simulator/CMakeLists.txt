cmake_minimum_required(VERSION 3.28.3)
#project(furnace_simulator)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

include(FetchContent)

# uSockets (required by uWebSockets)
FetchContent_Declare(
        uSockets
        GIT_REPOSITORY https://github.com/uNetworking/uSockets.git
        GIT_TAG v0.8.8
)
FetchContent_MakeAvailable(uSockets)

# uWebSockets (header-only, depends on uSockets)
FetchContent_Declare(
        uWebSockets
        GIT_REPOSITORY https://github.com/uNetworking/uWebSockets.git
        GIT_TAG v20.74.0
)
FetchContent_MakeAvailable(uWebSockets)

# Build uSockets as a static library
add_library(uSockets STATIC
        ${usockets_SOURCE_DIR}/src/libusockets.h
        ${usockets_SOURCE_DIR}/src/bsd.c
        ${usockets_SOURCE_DIR}/src/context.c
        ${usockets_SOURCE_DIR}/src/loop.c
        ${usockets_SOURCE_DIR}/src/socket.c
        ${usockets_SOURCE_DIR}/src/eventing/epoll_kqueue.c
        ${usockets_SOURCE_DIR}/src/crypto/openssl.c
        ${usockets_SOURCE_DIR}/src/crypto/sni_tree.cpp
)

target_include_directories(uSockets PUBLIC ${usockets_SOURCE_DIR}/src)
target_compile_definitions(uSockets PRIVATE LIBUS_USE_OPENSSL)
target_link_libraries(uSockets PUBLIC ssl crypto)

# HeatTreatFurnace is added by parent CMakeLists.txt

# nlohmann/json for JSON parsing
FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

add_executable(furnace_simulator
        src/main.cpp
        src/uWebSocketsTransport.cpp
        src/ThermalSimulator.cpp
        src/SimulatorStateProvider.cpp
        src/SimulatorProfileLoader.cpp
        src/FilesystemAssetProvider.cpp
)

target_include_directories(furnace_simulator PRIVATE
        ${uwebsockets_SOURCE_DIR}/src
        ${usockets_SOURCE_DIR}/src
)

target_link_libraries(furnace_simulator
        HeatTreatFurnace
        uSockets
        nlohmann_json::nlohmann_json
        z
)

set_target_properties(furnace_simulator PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
)
