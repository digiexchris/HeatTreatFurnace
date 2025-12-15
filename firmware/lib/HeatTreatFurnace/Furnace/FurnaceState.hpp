#ifndef HEAT_TREAT_FURNACE_FURNACE_HPP
#define HEAT_TREAT_FURNACE_FURNACE_HPP
#include <memory>

#include "../Profile/Profile.hpp"

namespace HeatTreatFurnace::Furnace
{
    struct FurnaceState
    {
        std::unique_ptr<Profile> myLoadedProgram;
        bool programIsRunning;
    };
}

#endif