# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 4.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

#FetchContent_Declare(
#        Catch2
#        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
#        GIT_TAG v3.8.1 # or a later release
#)

#FetchContent_MakeAvailable(Catch2)

include(FetchContent)
FetchContent_Declare(
        DocTest
        GIT_REPOSITORY "https://github.com/doctest/doctest"
        GIT_TAG "1da23a3e8119ec5cce4f9388e91b065e20bf06f5"
)

FetchContent_MakeAvailable(DocTest)

FetchContent_Declare(
        trompeloeil
        GIT_REPOSITORY https://github.com/rollbear/trompeloeil.git
        GIT_TAG v49)

FetchContent_MakeAvailable(trompeloeil)

project(test_app)

add_subdirectory(../lib/HeatTreatFurnace HeatTreatFurnace)
#add_subdirectory(lib/doctest-2.4.12 doctest)

add_executable(test_app
        main/test.cpp
    main/FSM/test_CancelledState.cpp
    main/FSM/test_CompletedState.cpp
    main/FSM/test_ErrorState.cpp
    main/FSM/test_IdleState.cpp
    main/FSM/test_LoadedState.cpp
    main/FSM/test_ManualTempState.cpp
    main/FSM/test_PausedState.cpp
    main/FSM/test_ProfileTempOverrideState.cpp
    main/FSM/test_RunningState.cpp
)

target_link_libraries(test_app
        HeatTreatFurnace
        doctest::doctest
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



