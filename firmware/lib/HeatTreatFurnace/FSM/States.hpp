// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Chris Sutton

/**
 * @file States.hpp
 * @brief State class declarations for the ETL-based FSM
 *
 * Defines all state classes that implement the furnace state machine behavior.
 * Each state derives from etl::fsm_state and implements on_event() handlers for
 * the messages it accepts. Event handlers return the next state ID.
 *
 * State responsibilities:
 * - Handle incoming events via on_event() methods
 * - Return target state ID (or No_State_Change to stay, or Pass_To_Parent)
 * - Perform entry actions via on_enter_state()
 * - Perform exit actions via on_exit_state()
 * - Handle unknown events via on_event_unknown()
 *
 * All states have access to FSM context which provides FurnaceState and LogService.
 *
 * State IDs must match their position in the state array (0-based):
 * 0=IDLE, 1=LOADED, 2=RUNNING, 3=PAUSED, 4=COMPLETED, 5=CANCELLED,
 * 6=ERROR, 7=MANUAL_TEMP, 8=PROFILE_TEMP_OVERRIDE
 */

#pragma once

#include "Events.hpp"
#include "FurnaceFsm.hpp"
#include "../Furnace/State.hpp"

#include <etl/fsm.h>

namespace HeatTreatFurnace
{
namespace FSM
{

// State ID constants matching Furnace::StateId enum
constexpr etl::fsm_state_id_t STATE_IDLE = static_cast<etl::fsm_state_id_t>(Furnace::StateId::IDLE);
constexpr etl::fsm_state_id_t STATE_LOADED = static_cast<etl::fsm_state_id_t>(Furnace::StateId::LOADED);
constexpr etl::fsm_state_id_t STATE_RUNNING = static_cast<etl::fsm_state_id_t>(Furnace::StateId::RUNNING);
constexpr etl::fsm_state_id_t STATE_PAUSED = static_cast<etl::fsm_state_id_t>(Furnace::StateId::PAUSED);
constexpr etl::fsm_state_id_t STATE_COMPLETED = static_cast<etl::fsm_state_id_t>(Furnace::StateId::COMPLETED);
constexpr etl::fsm_state_id_t STATE_CANCELLED = static_cast<etl::fsm_state_id_t>(Furnace::StateId::CANCELLED);
constexpr etl::fsm_state_id_t STATE_ERROR = static_cast<etl::fsm_state_id_t>(Furnace::StateId::ERROR);
constexpr etl::fsm_state_id_t STATE_MANUAL_TEMP = static_cast<etl::fsm_state_id_t>(Furnace::StateId::MANUAL_TEMP);
constexpr etl::fsm_state_id_t STATE_PROFILE_TEMP_OVERRIDE = static_cast<etl::fsm_state_id_t>(Furnace::StateId::PROFILE_TEMP_OVERRIDE);

/**
 * @brief IDLE state - No program loaded
 *
 * Valid transitions:
 * - EvtLoadProfile -> LOADED
 * - EvtSetManualTemp -> MANUAL_TEMP
 * - EvtError -> ERROR
 * - EvtTick -> (stays in IDLE)
 */
class StIdle : public etl::fsm_state<FurnaceFsm, StIdle, STATE_IDLE,
                                      EvtLoadProfile, EvtSetManualTemp, EvtError, EvtTick>
{
public:
    etl::fsm_state_id_t on_enter_state() override;
    void on_exit_state() override;

    etl::fsm_state_id_t on_event(EvtLoadProfile const& anEvent);
    etl::fsm_state_id_t on_event(EvtSetManualTemp const& anEvent);
    etl::fsm_state_id_t on_event(EvtError const& anEvent);
    etl::fsm_state_id_t on_event(EvtTick const& anEvent);

    etl::fsm_state_id_t on_event_unknown(etl::imessage const& aMsg);
};

/**
 * @brief LOADED state - Program loaded, ready to start
 *
 * Valid transitions:
 * - EvtStart -> RUNNING
 * - EvtLoadProfile -> LOADED (replace profile)
 * - EvtClearProgram -> IDLE
 * - EvtSetManualTemp -> MANUAL_TEMP
 * - EvtError -> ERROR
 * - EvtTick -> (stays in LOADED)
 */
class StLoaded : public etl::fsm_state<FurnaceFsm, StLoaded, STATE_LOADED,
                                        EvtStart, EvtLoadProfile, EvtClearProgram,
                                        EvtSetManualTemp, EvtError, EvtTick>
{
public:
    etl::fsm_state_id_t on_enter_state() override;
    void on_exit_state() override;

    etl::fsm_state_id_t on_event(EvtStart const& anEvent);
    etl::fsm_state_id_t on_event(EvtLoadProfile const& anEvent);
    etl::fsm_state_id_t on_event(EvtClearProgram const& anEvent);
    etl::fsm_state_id_t on_event(EvtSetManualTemp const& anEvent);
    etl::fsm_state_id_t on_event(EvtError const& anEvent);
    etl::fsm_state_id_t on_event(EvtTick const& anEvent);

    etl::fsm_state_id_t on_event_unknown(etl::imessage const& aMsg);
};

/**
 * @brief RUNNING state - Program executing
 *
 * Valid transitions:
 * - EvtPause -> PAUSED
 * - EvtComplete -> COMPLETED
 * - EvtCancel -> CANCELLED
 * - EvtSetManualTemp -> PROFILE_TEMP_OVERRIDE
 * - EvtError -> ERROR
 * - EvtTick -> (stays in RUNNING)
 */
class StRunning : public etl::fsm_state<FurnaceFsm, StRunning, STATE_RUNNING,
                                         EvtPause, EvtComplete, EvtCancel,
                                         EvtSetManualTemp, EvtError, EvtTick>
{
public:
    etl::fsm_state_id_t on_enter_state() override;
    void on_exit_state() override;

    etl::fsm_state_id_t on_event(EvtPause const& anEvent);
    etl::fsm_state_id_t on_event(EvtComplete const& anEvent);
    etl::fsm_state_id_t on_event(EvtCancel const& anEvent);
    etl::fsm_state_id_t on_event(EvtSetManualTemp const& anEvent);
    etl::fsm_state_id_t on_event(EvtError const& anEvent);
    etl::fsm_state_id_t on_event(EvtTick const& anEvent);

    etl::fsm_state_id_t on_event_unknown(etl::imessage const& aMsg);
};

/**
 * @brief PAUSED state - Program paused, maintaining current temperature
 *
 * Valid transitions:
 * - EvtResume -> RUNNING
 * - EvtCancel -> CANCELLED
 * - EvtError -> ERROR
 * - EvtTick -> (stays in PAUSED)
 */
class StPaused : public etl::fsm_state<FurnaceFsm, StPaused, STATE_PAUSED,
                                        EvtResume, EvtCancel, EvtError, EvtTick>
{
public:
    etl::fsm_state_id_t on_enter_state() override;
    void on_exit_state() override;

    etl::fsm_state_id_t on_event(EvtResume const& anEvent);
    etl::fsm_state_id_t on_event(EvtCancel const& anEvent);
    etl::fsm_state_id_t on_event(EvtError const& anEvent);
    etl::fsm_state_id_t on_event(EvtTick const& anEvent);

    etl::fsm_state_id_t on_event_unknown(etl::imessage const& aMsg);
};

/**
 * @brief COMPLETED state - Program finished successfully
 *
 * Valid transitions:
 * - EvtClearProgram -> IDLE
 * - EvtLoadProfile -> LOADED
 * - EvtSetManualTemp -> MANUAL_TEMP
 * - EvtError -> ERROR
 * - EvtTick -> (stays in COMPLETED)
 */
class StCompleted : public etl::fsm_state<FurnaceFsm, StCompleted, STATE_COMPLETED,
                                           EvtClearProgram, EvtLoadProfile,
                                           EvtSetManualTemp, EvtError, EvtTick>
{
public:
    etl::fsm_state_id_t on_enter_state() override;
    void on_exit_state() override;

    etl::fsm_state_id_t on_event(EvtClearProgram const& anEvent);
    etl::fsm_state_id_t on_event(EvtLoadProfile const& anEvent);
    etl::fsm_state_id_t on_event(EvtSetManualTemp const& anEvent);
    etl::fsm_state_id_t on_event(EvtError const& anEvent);
    etl::fsm_state_id_t on_event(EvtTick const& anEvent);

    etl::fsm_state_id_t on_event_unknown(etl::imessage const& aMsg);
};

/**
 * @brief CANCELLED state - Program cancelled by user
 *
 * Valid transitions:
 * - EvtLoadProfile -> LOADED
 * - EvtClearProgram -> IDLE
 * - EvtError -> ERROR
 * - EvtTick -> (stays in CANCELLED)
 */
class StCancelled : public etl::fsm_state<FurnaceFsm, StCancelled, STATE_CANCELLED,
                                           EvtLoadProfile, EvtClearProgram, EvtError, EvtTick>
{
public:
    etl::fsm_state_id_t on_enter_state() override;
    void on_exit_state() override;

    etl::fsm_state_id_t on_event(EvtLoadProfile const& anEvent);
    etl::fsm_state_id_t on_event(EvtClearProgram const& anEvent);
    etl::fsm_state_id_t on_event(EvtError const& anEvent);
    etl::fsm_state_id_t on_event(EvtTick const& anEvent);

    etl::fsm_state_id_t on_event_unknown(etl::imessage const& aMsg);
};

/**
 * @brief ERROR state - System error, requires reset
 *
 * Valid transitions:
 * - EvtReset -> IDLE
 * - EvtLoadProfile -> LOADED (recovery path)
 * - EvtError -> (stays in ERROR, logs additional error)
 * - EvtTick -> (stays in ERROR)
 */
class StError : public etl::fsm_state<FurnaceFsm, StError, STATE_ERROR,
                                       EvtReset, EvtLoadProfile, EvtError, EvtTick>
{
public:
    etl::fsm_state_id_t on_enter_state() override;
    void on_exit_state() override;

    etl::fsm_state_id_t on_event(EvtReset const& anEvent);
    etl::fsm_state_id_t on_event(EvtLoadProfile const& anEvent);
    etl::fsm_state_id_t on_event(EvtError const& anEvent);
    etl::fsm_state_id_t on_event(EvtTick const& anEvent);

    etl::fsm_state_id_t on_event_unknown(etl::imessage const& aMsg);
};

/**
 * @brief MANUAL_TEMP state - Manual temperature control (no program running)
 *
 * Valid transitions:
 * - EvtResume -> LOADED (if profile present) or IDLE
 * - EvtSetManualTemp -> MANUAL_TEMP (update setpoint)
 * - EvtError -> ERROR
 * - EvtTick -> (stays in MANUAL_TEMP)
 */
class StManualTemp : public etl::fsm_state<FurnaceFsm, StManualTemp, STATE_MANUAL_TEMP,
                                            EvtResume, EvtSetManualTemp, EvtError, EvtTick>
{
public:
    etl::fsm_state_id_t on_enter_state() override;
    void on_exit_state() override;

    etl::fsm_state_id_t on_event(EvtResume const& anEvent);
    etl::fsm_state_id_t on_event(EvtSetManualTemp const& anEvent);
    etl::fsm_state_id_t on_event(EvtError const& anEvent);
    etl::fsm_state_id_t on_event(EvtTick const& anEvent);

    etl::fsm_state_id_t on_event_unknown(etl::imessage const& aMsg);
};

/**
 * @brief PROFILE_TEMP_OVERRIDE state - Manual temp override during program
 *
 * Valid transitions:
 * - EvtResume -> RUNNING
 * - EvtSetManualTemp -> PROFILE_TEMP_OVERRIDE (update setpoint)
 * - EvtError -> ERROR
 * - EvtTick -> (stays in PROFILE_TEMP_OVERRIDE)
 */
class StProfileTempOverride : public etl::fsm_state<FurnaceFsm, StProfileTempOverride, STATE_PROFILE_TEMP_OVERRIDE,
                                                     EvtResume, EvtSetManualTemp, EvtError, EvtTick>
{
public:
    etl::fsm_state_id_t on_enter_state() override;
    void on_exit_state() override;

    etl::fsm_state_id_t on_event(EvtResume const& anEvent);
    etl::fsm_state_id_t on_event(EvtSetManualTemp const& anEvent);
    etl::fsm_state_id_t on_event(EvtError const& anEvent);
    etl::fsm_state_id_t on_event(EvtTick const& anEvent);

    etl::fsm_state_id_t on_event_unknown(etl::imessage const& aMsg);
};

}  // namespace FSM
}  // namespace HeatTreatFurnace
