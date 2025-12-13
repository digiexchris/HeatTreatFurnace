// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Chris Sutton

#include "States.hpp"

namespace HeatTreatFurnace
{
namespace FSM
{

//=============================================================================
// StIdle Implementation
//=============================================================================

etl::fsm_state_id_t StIdle::on_enter_state()
{
    FurnaceFsm& fsm = get_fsm_context();
    // Log entry to IDLE state
    return No_State_Change;
}

void StIdle::on_exit_state()
{
    // Cleanup if needed
}

etl::fsm_state_id_t StIdle::on_event(EvtLoadProfile const& anEvent)
{
    FurnaceFsm& fsm = get_fsm_context();
    // TODO: Validate profile
    // TODO: Store profile reference in FurnaceState
    // Log: Profile loaded
    return STATE_LOADED;
}

etl::fsm_state_id_t StIdle::on_event(EvtSetManualTemp const& anEvent)
{
    FurnaceFsm& fsm = get_fsm_context();
    // TODO: Validate temperature bounds
    // TODO: Set manual temperature control
    return STATE_MANUAL_TEMP;
}

etl::fsm_state_id_t StIdle::on_event(EvtError const& anEvent)
{
    FurnaceFsm& fsm = get_fsm_context();
    // Log error details
    return STATE_ERROR;
}

etl::fsm_state_id_t StIdle::on_event(EvtTick const& anEvent)
{
    // Emit current furnace state to observers
    return No_State_Change;
}

etl::fsm_state_id_t StIdle::on_event_unknown(etl::imessage const& aMsg)
{
    // Log invalid event and route to ERROR
    return STATE_ERROR;
}

//=============================================================================
// StLoaded Implementation
//=============================================================================

etl::fsm_state_id_t StLoaded::on_enter_state()
{
    FurnaceFsm& fsm = get_fsm_context();
    // Log entry to LOADED state
    return No_State_Change;
}

void StLoaded::on_exit_state()
{
    // Cleanup if needed
}

etl::fsm_state_id_t StLoaded::on_event(EvtStart const& anEvent)
{
    FurnaceFsm& fsm = get_fsm_context();
    // TODO: Check safety interlocks
    // TODO: Check sensors ready
    // TODO: Check controller ready
    // If checks fail, return STATE_ERROR
    // TODO: Initialize timers and logging
    return STATE_RUNNING;
}

etl::fsm_state_id_t StLoaded::on_event(EvtLoadProfile const& anEvent)
{
    FurnaceFsm& fsm = get_fsm_context();
    // TODO: Replace existing profile
    // Log: Profile replaced
    return No_State_Change;  // Stay in LOADED with new profile
}

etl::fsm_state_id_t StLoaded::on_event(EvtClearProgram const& anEvent)
{
    FurnaceFsm& fsm = get_fsm_context();
    // TODO: Clear profile from FurnaceState
    // Log: Program cleared
    return STATE_IDLE;
}

etl::fsm_state_id_t StLoaded::on_event(EvtSetManualTemp const& anEvent)
{
    FurnaceFsm& fsm = get_fsm_context();
    // TODO: Validate temperature bounds
    // TODO: Set manual temperature control
    return STATE_MANUAL_TEMP;
}

etl::fsm_state_id_t StLoaded::on_event(EvtError const& anEvent)
{
    FurnaceFsm& fsm = get_fsm_context();
    // Log error details
    return STATE_ERROR;
}

etl::fsm_state_id_t StLoaded::on_event(EvtTick const& anEvent)
{
    // Emit current furnace state to observers
    return No_State_Change;
}

etl::fsm_state_id_t StLoaded::on_event_unknown(etl::imessage const& aMsg)
{
    // Log invalid event and route to ERROR
    return STATE_ERROR;
}

//=============================================================================
// StRunning Implementation
//=============================================================================

etl::fsm_state_id_t StRunning::on_enter_state()
{
    FurnaceFsm& fsm = get_fsm_context();
    // TODO: Set programIsRunning = true
    // TODO: Start control loop
    // Log: Program started
    return No_State_Change;
}

void StRunning::on_exit_state()
{
    // TODO: Stop control loop if transitioning to non-override state
}

etl::fsm_state_id_t StRunning::on_event(EvtPause const& anEvent)
{
    FurnaceFsm& fsm = get_fsm_context();
    // TODO: Suspend profile execution
    // TODO: Maintain current temperature
    return STATE_PAUSED;
}

etl::fsm_state_id_t StRunning::on_event(EvtComplete const& anEvent)
{
    FurnaceFsm& fsm = get_fsm_context();
    // TODO: Stop control loop
    // TODO: Safe shutdown
    return STATE_COMPLETED;
}

etl::fsm_state_id_t StRunning::on_event(EvtCancel const& anEvent)
{
    FurnaceFsm& fsm = get_fsm_context();
    // TODO: Abort program
    // TODO: Safe shutdown
    return STATE_CANCELLED;
}

etl::fsm_state_id_t StRunning::on_event(EvtSetManualTemp const& anEvent)
{
    FurnaceFsm& fsm = get_fsm_context();
    // TODO: Validate temperature bounds
    // TODO: Hold at manual setpoint
    // Note: programIsRunning stays true
    return STATE_PROFILE_TEMP_OVERRIDE;
}

etl::fsm_state_id_t StRunning::on_event(EvtError const& anEvent)
{
    FurnaceFsm& fsm = get_fsm_context();
    // TODO: Stop control loop
    // Log error details
    return STATE_ERROR;
}

etl::fsm_state_id_t StRunning::on_event(EvtTick const& anEvent)
{
    // Emit current furnace state to observers
    return No_State_Change;
}

etl::fsm_state_id_t StRunning::on_event_unknown(etl::imessage const& aMsg)
{
    // Log invalid event and route to ERROR
    return STATE_ERROR;
}

//=============================================================================
// StPaused Implementation
//=============================================================================

etl::fsm_state_id_t StPaused::on_enter_state()
{
    FurnaceFsm& fsm = get_fsm_context();
    // Log: Program paused
    return No_State_Change;
}

void StPaused::on_exit_state()
{
    // Cleanup if needed
}

etl::fsm_state_id_t StPaused::on_event(EvtResume const& anEvent)
{
    FurnaceFsm& fsm = get_fsm_context();
    // TODO: Resume profile execution
    return STATE_RUNNING;
}

etl::fsm_state_id_t StPaused::on_event(EvtCancel const& anEvent)
{
    FurnaceFsm& fsm = get_fsm_context();
    // TODO: Abort program
    // TODO: Safe shutdown
    return STATE_CANCELLED;
}

etl::fsm_state_id_t StPaused::on_event(EvtError const& anEvent)
{
    FurnaceFsm& fsm = get_fsm_context();
    // TODO: Stop control loop
    // Log error details
    return STATE_ERROR;
}

etl::fsm_state_id_t StPaused::on_event(EvtTick const& anEvent)
{
    // Emit current furnace state to observers
    return No_State_Change;
}

etl::fsm_state_id_t StPaused::on_event_unknown(etl::imessage const& aMsg)
{
    // Log invalid event and route to ERROR
    return STATE_ERROR;
}

//=============================================================================
// StCompleted Implementation
//=============================================================================

etl::fsm_state_id_t StCompleted::on_enter_state()
{
    FurnaceFsm& fsm = get_fsm_context();
    // TODO: Set programIsRunning = false
    // Log: Program completed
    return No_State_Change;
}

void StCompleted::on_exit_state()
{
    // Cleanup if needed
}

etl::fsm_state_id_t StCompleted::on_event(EvtClearProgram const& anEvent)
{
    FurnaceFsm& fsm = get_fsm_context();
    // TODO: Clear profile from FurnaceState
    return STATE_IDLE;
}

etl::fsm_state_id_t StCompleted::on_event(EvtLoadProfile const& anEvent)
{
    FurnaceFsm& fsm = get_fsm_context();
    // TODO: Load new profile
    return STATE_LOADED;
}

etl::fsm_state_id_t StCompleted::on_event(EvtSetManualTemp const& anEvent)
{
    FurnaceFsm& fsm = get_fsm_context();
    // TODO: Validate temperature bounds
    // TODO: Set manual temperature control
    return STATE_MANUAL_TEMP;
}

etl::fsm_state_id_t StCompleted::on_event(EvtError const& anEvent)
{
    FurnaceFsm& fsm = get_fsm_context();
    // Log error details
    return STATE_ERROR;
}

etl::fsm_state_id_t StCompleted::on_event(EvtTick const& anEvent)
{
    // Emit current furnace state to observers
    return No_State_Change;
}

etl::fsm_state_id_t StCompleted::on_event_unknown(etl::imessage const& aMsg)
{
    // Log invalid event and route to ERROR
    return STATE_ERROR;
}

//=============================================================================
// StCancelled Implementation
//=============================================================================

etl::fsm_state_id_t StCancelled::on_enter_state()
{
    FurnaceFsm& fsm = get_fsm_context();
    // TODO: Set programIsRunning = false
    // Log: Program cancelled
    return No_State_Change;
}

void StCancelled::on_exit_state()
{
    // Cleanup if needed
}

etl::fsm_state_id_t StCancelled::on_event(EvtLoadProfile const& anEvent)
{
    FurnaceFsm& fsm = get_fsm_context();
    // TODO: Load new profile
    return STATE_LOADED;
}

etl::fsm_state_id_t StCancelled::on_event(EvtClearProgram const& anEvent)
{
    FurnaceFsm& fsm = get_fsm_context();
    // TODO: Clear profile from FurnaceState
    return STATE_IDLE;
}

etl::fsm_state_id_t StCancelled::on_event(EvtError const& anEvent)
{
    FurnaceFsm& fsm = get_fsm_context();
    // Log error details
    return STATE_ERROR;
}

etl::fsm_state_id_t StCancelled::on_event(EvtTick const& anEvent)
{
    // Emit current furnace state to observers
    return No_State_Change;
}

etl::fsm_state_id_t StCancelled::on_event_unknown(etl::imessage const& aMsg)
{
    // Log invalid event and route to ERROR
    return STATE_ERROR;
}

//=============================================================================
// StError Implementation
//=============================================================================

etl::fsm_state_id_t StError::on_enter_state()
{
    FurnaceFsm& fsm = get_fsm_context();
    // TODO: Stop all control operations
    // TODO: Set programIsRunning = false
    // Log: Entered ERROR state
    return No_State_Change;
}

void StError::on_exit_state()
{
    // TODO: Clear error flags
}

etl::fsm_state_id_t StError::on_event(EvtReset const& anEvent)
{
    FurnaceFsm& fsm = get_fsm_context();
    // TODO: Clear error flags
    // TODO: Clear program from FurnaceState
    // Log: System reset
    return STATE_IDLE;
}

etl::fsm_state_id_t StError::on_event(EvtLoadProfile const& anEvent)
{
    FurnaceFsm& fsm = get_fsm_context();
    // Recovery path: load profile directly from error
    // TODO: Validate profile
    // TODO: Clear error flags
    return STATE_LOADED;
}

etl::fsm_state_id_t StError::on_event(EvtError const& anEvent)
{
    FurnaceFsm& fsm = get_fsm_context();
    // Log additional error
    return No_State_Change;  // Stay in ERROR
}

etl::fsm_state_id_t StError::on_event(EvtTick const& anEvent)
{
    // Emit current furnace state to observers
    return No_State_Change;
}

etl::fsm_state_id_t StError::on_event_unknown(etl::imessage const& aMsg)
{
    // Log unknown event but stay in ERROR
    return No_State_Change;
}

//=============================================================================
// StManualTemp Implementation
//=============================================================================

etl::fsm_state_id_t StManualTemp::on_enter_state()
{
    FurnaceFsm& fsm = get_fsm_context();
    // TODO: Enable manual temperature control
    // Note: programIsRunning is NOT modified
    // Log: Entered manual temperature mode
    return No_State_Change;
}

void StManualTemp::on_exit_state()
{
    // TODO: Disable manual temperature control
}

etl::fsm_state_id_t StManualTemp::on_event(EvtResume const& anEvent)
{
    FurnaceFsm& fsm = get_fsm_context();
    Furnace::FurnaceState& furnaceState = fsm.GetFurnaceState();

    // If profile is loaded, go to LOADED; otherwise go to IDLE
    if (furnaceState.myLoadedProgram != nullptr)
    {
        return STATE_LOADED;
    }
    else
    {
        return STATE_IDLE;
    }
}

etl::fsm_state_id_t StManualTemp::on_event(EvtSetManualTemp const& anEvent)
{
    FurnaceFsm& fsm = get_fsm_context();
    // TODO: Update manual setpoint
    return No_State_Change;  // Stay in MANUAL_TEMP with new setpoint
}

etl::fsm_state_id_t StManualTemp::on_event(EvtError const& anEvent)
{
    FurnaceFsm& fsm = get_fsm_context();
    // Log error details
    return STATE_ERROR;
}

etl::fsm_state_id_t StManualTemp::on_event(EvtTick const& anEvent)
{
    // Emit current furnace state to observers
    return No_State_Change;
}

etl::fsm_state_id_t StManualTemp::on_event_unknown(etl::imessage const& aMsg)
{
    // Log invalid event and route to ERROR
    return STATE_ERROR;
}

//=============================================================================
// StProfileTempOverride Implementation
//=============================================================================

etl::fsm_state_id_t StProfileTempOverride::on_enter_state()
{
    FurnaceFsm& fsm = get_fsm_context();
    // TODO: Set manual temperature override
    // Note: programIsRunning stays true
    // Log: Temperature override active
    return No_State_Change;
}

void StProfileTempOverride::on_exit_state()
{
    // TODO: Clear manual temperature override
}

etl::fsm_state_id_t StProfileTempOverride::on_event(EvtResume const& anEvent)
{
    FurnaceFsm& fsm = get_fsm_context();
    // Resume program execution
    return STATE_RUNNING;
}

etl::fsm_state_id_t StProfileTempOverride::on_event(EvtSetManualTemp const& anEvent)
{
    FurnaceFsm& fsm = get_fsm_context();
    // TODO: Update override setpoint
    return No_State_Change;  // Stay in override with new setpoint
}

etl::fsm_state_id_t StProfileTempOverride::on_event(EvtError const& anEvent)
{
    FurnaceFsm& fsm = get_fsm_context();
    // Log error details
    return STATE_ERROR;
}

etl::fsm_state_id_t StProfileTempOverride::on_event(EvtTick const& anEvent)
{
    // Emit current furnace state to observers
    return No_State_Change;
}

etl::fsm_state_id_t StProfileTempOverride::on_event_unknown(etl::imessage const& aMsg)
{
    // Log invalid event and route to ERROR
    return STATE_ERROR;
}

}  // namespace FSM
}  // namespace HeatTreatFurnace
