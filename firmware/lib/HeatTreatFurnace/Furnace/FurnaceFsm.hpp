// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Chris Sutton

/**
 * @file FurnaceFsm.hpp
 * @brief ETL-based finite state machine for furnace control
 *
 * Implements the furnace state machine using ETL's FSM infrastructure with
 * event-driven transitions. The FSM derives from etl::fsm and manages state
 * transitions through event handlers that return the next state ID.
 *
 * The FSM owns:
 * - EventQueueManager for thread-safe event posting and draining
 * - References to FurnaceState (furnace operational data) and LogService
 *
 * States are external and registered via set_states(). Event handlers in states
 * return the next state ID, and ETL automatically handles on_exit/on_enter calls.
 *
 * Usage:
 *   FurnaceFsm fsm(furnaceState, logger);
 *   etl::ifsm_state* states[] = { &idle, &loaded, ... };
 *   fsm.set_states(states, numStates);
 *   fsm.start();
 *
 *   // Post events to queue
 *   EvtStart startEvent;
 *   fsm.Post(startEvent, EventPriority::UI);
 *
 *   // Process queue regularly
 *   fsm.ProcessQueue();
 */

#pragma once

#include "../FSM/EventQueueManager.hpp"
#include "../FSM/Types.hpp"
#include "Furnace/Furnace.hpp"
#include "States/StateId.hpp"
#include "Log/LogService.hpp"

#include "FSM/States/CancelledState.hpp"
#include "FSM/States/CompletedState.hpp"
#include "FSM/States/ErrorState.hpp"
#include "FSM/States/IdleState.hpp"
#include "FSM/States/LoadedState.hpp"
#include "FSM/States/ManualTempState.hpp"
#include "FSM/States/PausedState.hpp"
#include "FSM/States/ProfileTempOverrideState.hpp"
#include "FSM/States/RunningState.hpp"

#include <etl/fsm.h>


namespace HeatTreatFurnace::FSM
{
    const etl::message_router_id_t FURNACE_FSM_ROUTER = 0;
/**
 * @brief Main FSM class for furnace state management
 *
 * Derives from etl::fsm to handle state transitions automatically.
 * Overrides receive() to queue messages, then processes them via
 * etl::fsm::receive() during ProcessQueue().
 */
class FurnaceFsm : public etl::fsm, public Log::Loggable
{
public:
    FurnaceFsm(Furnace::FurnaceState& aFurnaceState, Log::LogService& aLogger);

    /**
     * @brief Overridden receive to queue messages instead of processing immediately
     * @param aMsg Message to queue for later processing
     */
    void receive(etl::imessage const& aMsg) override;

    void Init();

    /**
     * @brief Post an event to the queue with specified priority
     * @param aMsg Event message to post
     * @param aPriority Priority level for the event
     * @return true if posted successfully, false if queue is full
     */
    bool Post(etl::imessage const& aMsg, EventPriority aPriority);

    /**
     * @brief Process all queued events in priority order
     *
     * Drains the event queue and delivers each event to the FSM via
     * etl::fsm::receive(), which routes to current state handlers.
     * Should be called regularly from the main loop.
     */
    void ProcessQueue();

    template <typename... Args, typename StateType>
    void SendLog(Log::LogLevel aLevel, StateType& aState, const etl::string_view& aFormat, Args&&... aArgs)
    {
        myLogService.Log(aLevel, aState.Name(), aFormat, std::forward<Args>(aArgs)...);
    }

    /**
     * @brief Get the current state ID
     * @return Current state identifier (mapped to Furnace::StateId)
     */
    [[nodiscard]] StateId GetCurrentState() const noexcept;

    /**
     * @brief Get reference to FurnaceState for states to access
     * @return Reference to furnace operational state
     */
    Furnace::FurnaceState& GetFurnaceState() noexcept { return myFurnaceState; }

    /**
     * @brief Get reference to LogService for states to access
     * @return Reference to logging service
     */
    Log::LogService& GetLogger() noexcept { return myLogger; }

    /**
     * @brief Get the overflow counter from the queue manager
     * @return Number of events dropped due to queue overflow
     */
    [[nodiscard]] uint32_t GetOverflowCount() const noexcept;

    void Initialize();

private:
    EventQueueManager myQueueManager;
    Furnace::FurnaceState& myFurnaceState;
    Log::LogService& myLogger;
    etl::fsm_state_pack<IdleState,
                LoadedState,
                RunningState,
                PausedState,
                CompletedState,
                CancelledState,
                ErrorState,
                ManualTempState,
                ProfileTempOverrideState> myStatePack;
};

} // namespace HeatTreatFurnace::FSM

