#pragma once

#include "Furnace/Types.hpp"

#include <etl/message.h>
#include <etl/string.h>
#include <etl/fsm.h>

#include "StateId.hpp"
#include "Profile/Profile.hpp"

namespace HeatTreatFurnace::Furnace
{
    class FurnaceFsm;
    /**
     * @brief Event message IDs for ETL message routing
     */
    enum EventId : etl::message_id_t
    {
        EVENT_MODE_OFF = 0,
        EVENT_MODE_PROFILE,
        EVENT_MODE_MANUAL,
        EVENT_PROFILE_LOAD,
        EVENT_PROFILE_ALREADY_LOADED,
        EVENT_PROFILE_START,
        EVENT_PROFILE_SET_NEXT_SEGMENT, //Sets the next segment to execute, for instance to skip to a point in the program instead of starting at the beginning
        EVENT_PROFILE_STOP,
        EVENT_PROFILE_CLEAR,
        EVENT_PROFILE_COMPLETE,
        EVENT_MANUAL_SET_TEMP,
        EVENT_MANUAL_SET_ON,
        EVENT_MANUAL_SET_OFF,
        EVENT_TICK,
        EVENT_ERROR,
        NUM_EVENTS
    };

    struct PriorityQueueEvent : public etl::imessage
    {
        std::chrono::milliseconds timestamp;
        EventPriority priority = EventPriority::NONE;
        PriorityQueueEvent() : priority(EventPriority::NONE)
        {
            timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
        }
    };


    template<EventId E>
    struct EvtBase : public etl::message<E,PriorityQueueEvent>
    {
        EvtBase(EventPriority aPriority) : etl::message<E, PriorityQueueEvent>()
        {
            this->priority = aPriority;
        }
        EventId eventId = E;
    };

    /// Event to switch furnace to OFF mode, turn off the emergency relay, turn off the heaters.
    struct EvtModeOff : public EvtBase<EVENT_MODE_OFF>
    {
        EvtModeOff(): EvtBase(EventPriority::Furnace) {}
    };

    /// Event to switch furnace to PROFILE mode, auto transition to LOADED if a program was already loaded.
    struct EvtModeProfile : public EvtBase<EVENT_MODE_PROFILE>
    {
        EvtModeProfile(): EvtBase(EventPriority::Furnace) {}
    };

    /// Transitions to MANUAL using the current target. turns the heater on if off.
    struct EvtModeManual : public EvtBase<EVENT_MODE_MANUAL>
    {
        EvtModeManual(): EvtBase(EventPriority::Furnace) {}
    };

    /**
     * @brief Event to load a profile into the FSM
     *
     * Can be sent from any main mode, and non-running profile states.
     * It will only cause a state change if in the PROGRAM state other than LOADED.
     */
    struct EvtProfileLoad : public EvtBase<EVENT_PROFILE_LOAD>
    {
        Profile profile;

        explicit EvtProfileLoad(Profile const& aProfile) : EvtBase(EventPriority::Furnace), profile(aProfile)
        {
        }
    };

    struct EvtProfileComplete : public EvtBase<EVENT_PROFILE_COMPLETE>
    {
        EvtProfileComplete() : EvtBase(EventPriority::Furnace) {}
    };

    //Simple transition to Loaded from Profile root state
    struct EvtProfileAlreadyLoaded : public EvtBase<EVENT_PROFILE_ALREADY_LOADED>
    {
        EvtProfileAlreadyLoaded() : EvtBase(EventPriority::Furnace) {}
    };

    /// Event to start program execution from PROFILE, PROFILE_LOADED, PROFILE_STOPPED, PROFILE_COMPLETED state
    /// Continues profile if it was stopped in-progress (ie. paused)
    struct EvtProfileStart : public EvtBase<EVENT_PROFILE_START>
    {
        EvtProfileStart() : EvtBase(EventPriority::Furnace) {}
    };

    /// Event to set the next segment to execute in the profile
    /// in the completed state, transition to Loaded again.
    struct EvtProfileSetNextSegment : public EvtBase<EVENT_PROFILE_SET_NEXT_SEGMENT>
    {
        uint16_t segmentIndex;
        std::chrono::seconds segmentTime;

        explicit EvtProfileSetNextSegment(uint16_t aSegmentIndex, std::chrono::seconds aSegmentTime = std::chrono::seconds(0))
            : EvtBase(EventPriority::Furnace), segmentIndex(aSegmentIndex), segmentTime(aSegmentTime)
        {
        }
    };

    /// Stop the profile, hold the current temp target. ie. pause, or stop to switch to manual mode.
    /// If the user then switches to OFF, then the heater shuts off.
    struct EvtProfileStop : public EvtBase<EVENT_PROFILE_STOP>
    {
        EvtProfileStop() : EvtBase(EventPriority::Critical) {}
    };

    /// Event to clear the loaded program and return to the previous state if not in a PROFILE state
    /// If in a PROFILE sub-state, switch to PROFILE.
    struct EvtProfileClear : public EvtBase<EVENT_PROFILE_CLEAR>
    {
        EvtProfileClear() : EvtBase(EventPriority::Furnace) {}
    };

    /**
     * @brief Event indicating an error condition
     *
     * Routes the FSM to OFF->ERROR state with error details for logging and UI use.
     */
    struct EvtError : public EvtBase<EVENT_ERROR>
    {
        Error error;
        Domain domain;
        etl::string<96> msg;

        EvtError(Error anError, Domain aDomain, etl::string<1024> const& aMessage)
            : EvtBase(EventPriority::Critical), error(anError), domain(aDomain), msg(aMessage)
        {
        }
    };

    /**
     * @brief Event to set manual temperature control
     *
     * Transitions to MANUAL, and emits EVENT_HEATER_SET_TARGET, and EVENT_HEATER_ON.
     */
    struct EvtManualSetTemp : public EvtBase<EVENT_MANUAL_SET_TEMP>
    {
        float targetTemp;

        explicit EvtManualSetTemp(float aTargetTemp) : EvtBase(EventPriority::Furnace), targetTemp(aTargetTemp)
        {
        }
    };

    struct EvtManualSetOn : public EvtBase<EVENT_MANUAL_SET_ON>
    {
        EvtManualSetOn() : EvtBase(EventPriority::Furnace) {}
    };

    struct EvtManualSetOff : public EvtBase<EVENT_MANUAL_SET_OFF>
    {
        EvtManualSetOff() : EvtBase(EventPriority::Critical) {}
    };

    /// Event for periodic system tick updates
    /// currently used in the PROGRAM_RUNNING state to time when to do another
    /// update to move to the next temp target.
    struct EvtTick : public EvtBase<EVENT_TICK>
    {
        EvtTick() : EvtBase(EventPriority::Furnace) {}
    };
} // namespace HeatTreatFurnace::FSM
