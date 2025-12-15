#ifndef HEAT_TREAT_FURNACE_PROFILE_HPP
#define HEAT_TREAT_FURNACE_PROFILE_HPP

#include <chrono>
#include <string>
#include <vector>

namespace HeatTreatFurnace::Furnace
{
    /**
     * @brief A segment of the profile, describing how long to get to the target, and how long to hold it

    *
     * uses std::chrono
    auto one_hour = hours(1);
    auto three_minutes = minutes(3);
    auto combined = one_hour + three_minutes;  // 63 minutes
    using namespace std::chrono_literals;
    auto span = 1h + 3min + 30s;
     */

    class ProfileSegment
    {
        float target = 0.0f;
        std::chrono::milliseconds rampTime = std::chrono::milliseconds(0);
        std::chrono::milliseconds dwellTime = std::chrono::milliseconds(0);
    };

    /**
     * @brief Temperature profile to follow
     */
    struct Profile
    {
        std::string name;
        std::string description;
        std::vector<ProfileSegment> segments;
    };
} //HeatTreatFurnace::Furnace

#endif //HEAT_TREAT_FURNACE_PROFILE_HPP
