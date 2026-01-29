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

    inline etl::string<32> EventIdToString(EventId anEvtId)
    {
        switch (anEvtId)
        {
        case EVENT_MODE_OFF: return "EVENT_MODE_OFF";
        case EVENT_MODE_PROFILE: return "EVENT_MODE_PROFILE";
        case EVENT_MODE_MANUAL: return "EVENT_MODE_MANUAL";
        case EVENT_PROFILE_LOAD: return "EVENT_PROFILE_LOAD";
        case EVENT_PROFILE_ALREADY_LOADED: return "EVENT_PROFILE_ALREADY_LOADED";
        case EVENT_PROFILE_START: return "EVENT_PROFILE_START";
        case EVENT_PROFILE_SET_NEXT_SEGMENT: return "EVENT_PROFILE_SET_NEXT_SEGMENT";
        case EVENT_PROFILE_STOP: return "EVENT_PROFILE_STOP";
        case EVENT_PROFILE_CLEAR: return "EVENT_PROFILE_CLEAR";
        case EVENT_PROFILE_COMPLETE: return "EVENT_PROFILE_COMPLETE";
        case EVENT_MANUAL_SET_TEMP: return "EVENT_MANUAL_SET_TEMP";
        case EVENT_MANUAL_SET_ON: return "EVENT_MANUAL_SET_ON";
        case EVENT_MANUAL_SET_OFF: return "EVENT_MANUAL_SET_OFF";
        case EVENT_TICK: return "EVENT_TICK";
        case EVENT_ERROR: return "EVENT_ERROR";
        case NUM_EVENTS: return "NUM_EVENTS";
        default: return "UNKNOWN_EVENT";
        }
    }

    template <EventId E>

    struct EvtBase : public etl::message<E>
    {
        EvtBase() : etl::message<E>()
        {
        }

        EventId eventId = E;
    };

    /// Event to switch furnace to OFF mode, turn off the emergency relay, turn off the heaters.
    struct EvtModeOff : public EvtBase<EVENT_MODE_OFF>
    {
        EvtModeOff() : EvtBase()
        {
        }
    };

    /// Event to switch furnace to PROFILE mode, auto transition to LOADED if a program was already loaded.
    struct EvtModeProfile : public EvtBase<EVENT_MODE_PROFILE>
    {
        EvtModeProfile() : EvtBase()
        {
        }
    };

    /// Transitions to MANUAL using the current target. turns the heater on if off.
    struct EvtModeManual : public EvtBase<EVENT_MODE_MANUAL>
    {
        EvtModeManual() : EvtBase()
        {
        }
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

        explicit EvtProfileLoad(Profile const& aProfile) : EvtBase(), profile(aProfile)
        {
        }
    };

    struct EvtProfileComplete : public EvtBase<EVENT_PROFILE_COMPLETE>
    {
        EvtProfileComplete() : EvtBase()
        {
        }
    };

    //Simple transition to Loaded from Profile root state
    struct EvtProfileAlreadyLoaded : public EvtBase<EVENT_PROFILE_ALREADY_LOADED>
    {
        EvtProfileAlreadyLoaded() : EvtBase()
        {
        }
    };

    /// Event to start program execution from PROFILE, PROFILE_LOADED, PROFILE_STOPPED, PROFILE_COMPLETED state
    /// Continues profile if it was stopped in-progress (ie. paused)
    struct EvtProfileStart : public EvtBase<EVENT_PROFILE_START>
    {
        EvtProfileStart() : EvtBase()
        {
        }
    };

    /// Event to set the next segment to execute in the profile
    /// in the completed state, transition to Loaded again.
    struct EvtProfileSetNextSegment : public EvtBase<EVENT_PROFILE_SET_NEXT_SEGMENT>
    {
        uint16_t segmentIndex;
        std::chrono::seconds segmentTime;

        explicit EvtProfileSetNextSegment(uint16_t aSegmentIndex, std::chrono::seconds aSegmentTime = std::chrono::seconds(0))
            : EvtBase(), segmentIndex(aSegmentIndex), segmentTime(aSegmentTime)
        {
        }
    };

    /// Stop the profile, hold the current temp target. ie. pause, or stop to switch to manual mode.
    /// If the user then switches to OFF, then the heater shuts off.
    struct EvtProfileStop : public EvtBase<EVENT_PROFILE_STOP>
    {
        EvtProfileStop() : EvtBase()
        {
        }
    };

    /// Event to clear the loaded program and return to the previous state if not in a PROFILE state
    /// If in a PROFILE sub-state, switch to PROFILE.
    struct EvtProfileClear : public EvtBase<EVENT_PROFILE_CLEAR>
    {
        EvtProfileClear() : EvtBase()
        {
        }
    };

    /**
     * @brief Event indicating an error condition
     *
     * Routes the FSM to ERROR state with error details for logging and UI use.
     * When posted, flushes the queue and delivers directly to the FSM.
     */
    struct EvtError : public EvtBase<EVENT_ERROR>
    {
        Error error;
        Domain domain;
        etl::string<96> msg;

        EvtError(Error anError, Domain aDomain, etl::string<1024> const& aMessage)
            : EvtBase(), error(anError), domain(aDomain), msg(aMessage)
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

        explicit EvtManualSetTemp(float aTargetTemp) : EvtBase(), targetTemp(aTargetTemp)
        {
        }
    };

    struct EvtManualSetOn : public EvtBase<EVENT_MANUAL_SET_ON>
    {
        EvtManualSetOn() : EvtBase()
        {
        }
    };

    struct EvtManualSetOff : public EvtBase<EVENT_MANUAL_SET_OFF>
    {
        EvtManualSetOff() : EvtBase()
        {
        }
    };

    /// Event for periodic system tick updates
    /// currently used in the PROGRAM_RUNNING state to time when to do another
    /// update to move to the next temp target.
    struct EvtTick : public EvtBase<EVENT_TICK>
    {
        EvtTick() : EvtBase()
        {
        }
    };
} // namespace HeatTreatFurnace::FSM
