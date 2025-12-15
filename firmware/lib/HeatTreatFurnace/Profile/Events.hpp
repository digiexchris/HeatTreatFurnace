#pragma once
#include <etl/message.h>
#include <etl/string.h>

#include "Profile/Profile.hpp"

namespace HeatTreatFurnace::Profile
{
    /**
     * @brief Event message IDs for ETL message routing
     */
    enum EventId : etl::message_id_t
    {
        EVENT_LOAD_PROFILE = 0,
        EVENT_START,
        EVENT_SET_NEXT_SEGMENT, //Sets the next segment to execute, for instance to skip to a point in the program instead of starting at the beginning
        EVENT_STOP,
        EVENT_CLEAR_PROGRAM,
        EVENT_TICK
    };

    /**
     * @brief Event to load a profile into the FSM
     *
     * Holds a reference to a TempProfile object. The FSM does not own the profile;
     * ownership is managed externally.
     */
    struct EvtLoadProfile : etl::message<EVENT_LOAD_PROFILE>
    {
        Furnace::Profile const& profile;

        explicit EvtLoadProfile(Furnace::Profile const& aProfile) : profile(aProfile)
        {
        }
    };

    /// Event to start program execution from LOADED state
    struct EvtStart : public etl::message<EVENT_START>
    {
    };

    /// Event to cancel program execution
    struct EvtStop : public etl::message<EVENT_STOP>
    {
    };

    /// Event indicating program has completed successfully
    struct EvtSetNextSegment : public etl::message<EVENT_SET_NEXT_SEGMENT>
    {
        int16_t segmentIndex;
        std::chrono::seconds segmentTimePosition;

        EvtSetNextSegment(int16_t anIndex, std::chrono::seconds aTimePosition) : segmentIndex(anIndex), segmentTimePosition(aTimePosition)
        {
        }
    };

    /// Event to clear the loaded program and return to IDLE
    struct EvtClearProgram : public etl::message<EVENT_CLEAR_PROGRAM>
    {
    };

    struct EvtTick : public etl::message<EVENT_TICK>
    {
    };
} // namespace HeatTreatFurnace::FSM
