add_library(Furnace StateMachine.cpp
        Profile.hpp
        Action.hpp
        Furnace.cpp
        Furnace.hpp)

target_include_directories(Furnace PUBLIC .)