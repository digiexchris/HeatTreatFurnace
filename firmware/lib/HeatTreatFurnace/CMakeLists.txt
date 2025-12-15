# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 4.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# FetchContent_Declare(
#        fmt
#        GIT_REPOSITORY https://github.com/fmtlib/fmt
#        GIT_TAG e69e5f977d458f2650bb346dadf2ad30c5320281) # 10.2.1
#FetchContent_MakeAvailable(fmt)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../etl-20.44.2 ETLCPP)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../fmt-12.1.0 FMT)

add_library(HeatTreatFurnace
        Furnace/EventQueueManager.cpp
        Furnace/FurnaceFsm.cpp
        Furnace/Profile/ProfileStoppedState.cpp
        Furnace/Profile/ProfileCompletedState.cpp
        Furnace/Mode/ErrorState.cpp
        Furnace/Profile/ProfileLoadedState.cpp
        Furnace/Mode/ManualState.cpp
        Furnace/Profile/Profile.hpp
        Log/LogBackend.cpp
        Log/LogBackend.hpp
        Log/LogService.cpp
        Log/LogService.hpp
        Log/ConsoleLogBackend.cpp
        Log/ConsoleLogBackend.hpp
)

target_link_libraries(HeatTreatFurnace PUBLIC etl::etl fmt::fmt-header-only)

set_property(TARGET HeatTreatFurnace PROPERTY CXX_STANDARD 23)
target_include_directories(HeatTreatFurnace PUBLIC .)



