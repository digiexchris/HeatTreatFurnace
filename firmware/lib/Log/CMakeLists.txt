add_library(Log Log.cpp
        Log.hpp)

target_include_directories(Log PUBLIC .)

include(FetchContent)

FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG        486b55554f11c9cccc913e11a87085b2a91f706f
        GIT_PROGRESS   TRUE
        GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(spdlog)
target_link_libraries(Log INTERFACE spdlog::spdlog_header_only)