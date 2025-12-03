# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.8.1 # or a later release
)

FetchContent_MakeAvailable(Catch2)

FetchContent_Declare(
        trompeloeil
        GIT_REPOSITORY https://github.com/rollbear/trompeloeil.git
        GIT_TAG v43)

FetchContent_MakeAvailable(trompeloeil)

project(test_app)

add_subdirectory(../lib/Furnace Furnace)
add_subdirectory(../lib/Log Log)

add_executable(test_app
        main/test_StateMachine.cpp
)

target_link_libraries(test_app
        Furnace
        Log
        Catch2::Catch2WithMain
        trompeloeil::trompeloeil
)

target_include_directories(test_app PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(test_app PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
)
# endif()



