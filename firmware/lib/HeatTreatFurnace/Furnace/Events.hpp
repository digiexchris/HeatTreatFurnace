#pragma once

#include "Furnace/Types.hpp"

#include <etl/message.h>
#include <etl/string.h>

#include <cstdint>

#include "Profile/Profile.hpp"

namespace HeatTreatFurnace::Furnace
{
    /**
     * @brief Event message IDs for ETL message routing
     */
    enum EventId : etl::message_id_t
    {
        EVENT_LOAD_PROFILE = 0,
        EVENT_START,
        EVENT_PAUSE,
        EVENT_RESUME,
        EVENT_CANCEL,
        EVENT_COMPLETE,
        EVENT_CLEAR_PROGRAM,
        EVENT_ERROR,
        EVENT_RESET,
        EVENT_SET_MANUAL_TEMP,
        EVENT_SET_HEATER_TARGET,
        EVENT_SET_HEATER_ON,
        EVENT_SET_HEATER_OFF,
        EVENT_TICK
    };

    /**
     * @brief Event to load a profile into the FSM
     *
     * Holds a reference to a Profile object. The FSM does not own the profile;
     * ownership is managed externally.
     */
    struct EvtLoadProfile : public etl::message<EVENT_LOAD_PROFILE>
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

    /// Event to pause program execution from RUNNING state
    struct EvtPause : public etl::message<EVENT_PAUSE>
    {
    };

    /// Event to resume program execution from PAUSED or manual temp states
    struct EvtResume : public etl::message<EVENT_RESUME>
    {
    };

    /// Event to cancel program execution
    struct EvtCancel : public etl::message<EVENT_CANCEL>
    {
    };

    /// Event indicating program has completed successfully
    struct EvtComplete : public etl::message<EVENT_COMPLETE>
    {
    };

    /// Event to clear the loaded program and return to IDLE
    struct EvtClearProgram : public etl::message<EVENT_CLEAR_PROGRAM>
    {
    };

    /**
     * @brief Event indicating an error condition
     *
     * Routes the FSM to ERROR state with error details for logging.
     */
    struct EvtError : public etl::message<EVENT_ERROR>
    {
        Error error;
        Domain domain;
        etl::string<96> msg;

        EvtError(Error anError, Domain aDomain, etl::string<96> const& aMessage)
            : error(anError), domain(aDomain), msg(aMessage)
        {
        }
    };

    /// Event to reset the FSM from ERROR state back to IDLE
    struct EvtReset : public etl::message<EVENT_RESET>
    {
    };

    /**
     * @brief Event to set manual temperature control
     *
     * Transitions to MANUAL_TEMP or PROFILE_TEMP_OVERRIDE depending on current state.
     */
    struct EvtSetManualTemp : public etl::message<EVENT_SET_MANUAL_TEMP>
    {
        float targetTemp;

        explicit EvtSetManualTemp(float aTargetTemp) : targetTemp(aTargetTemp)
        {
        }
    };

    struct EvtSetHeaterOn : etl::message<EVENT_SET_HEATER_ON>
    {
    };

    struct EvtSetHeaterOff : etl::message<EVENT_SET_HEATER_OFF>
    {
    };

    struct EvtSetHeaterTarget : etl::message<EVENT_SET_HEATER_TARGET>
    {
        float targetTemp;

        explicit EvtSetHeaterTarget(float aTargetTemp) : targetTemp(aTargetTemp)
        {
        }
    };
} // namespace HeatTreatFurnace::FSM
