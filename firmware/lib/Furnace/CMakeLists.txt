add_library(Furnace StateMachine.cpp
        Profile.hpp
        Action.hpp
        Furnace.cpp
        Furnace.hpp)

add_subdirectory(../Log Log)
set_property(TARGET Furnace PROPERTY CXX_STANDARD 23)
target_include_directories(Furnace PUBLIC .)

target_link_libraries(Furnace Log)