# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 4.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# FetchContent_Declare(
#        fmt
#        GIT_REPOSITORY https://github.com/fmtlib/fmt
#        GIT_TAG e69e5f977d458f2650bb346dadf2ad30c5320281) # 10.2.1
#FetchContent_MakeAvailable(fmt)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../etl-20.44.1 ETLCPP)

add_library(HeatTreatFurnace
        Furnace/StateMachine.cpp
        Furnace/Profile.hpp
        Furnace/Action.hpp
        Furnace/Furnace.cpp
        Furnace/Furnace.hpp
        Log/LogBackend.cpp
        Log/LogBackend.hpp
        Log/LogService.cpp
        Log/LogService.hpp
        Log/ConsoleLogBackend.cpp
        Log/ConsoleLogBackend.hpp
)

target_link_libraries(HeatTreatFurnace PUBLIC etl::etl)

set_property(TARGET HeatTreatFurnace PROPERTY CXX_STANDARD 23)
target_include_directories(HeatTreatFurnace PUBLIC .)



