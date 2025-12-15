#ifndef HEAT_TREAT_FURNACE_PROFILE_HPP
#define HEAT_TREAT_FURNACE_PROFILE_HPP

#include <chrono>
#include <string>
#include <vector>

namespace HeatTreatFurnace::Profile
{
    constexpr size_t MAX_PROFILE_SEGMENTS = 32;
    constexpr size_t MAX_PROFILE_NAME_LEN = 64;
    constexpr size_t MAX_PROFILE_DESC_LEN = 1024;
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
        std::chrono::seconds rampTime = std::chrono::seconds(0);
        std::chrono::seconds dwellTime = std::chrono::seconds(0);
    };

    /**
     * @brief Temperature profile to follow
     */
    struct Profile
    {
        etl::string<MAX_PROFILE_NAME_LEN> name;
        etl::string<MAX_PROFILE_DESC_LEN> description;
        etl::vector<ProfileSegment, MAX_PROFILE_SEGMENTS> segments;
        uint16_t currentSegment = 0; //not saved to disk
        std::chrono::seconds currentSegmentTime = std::chrono::seconds(0); //not saved to disk. the current position within the current segment
    };
} //HeatTreatFurnace::Profile

#endif //HEAT_TREAT_FURNACE_PROFILE_HPP
