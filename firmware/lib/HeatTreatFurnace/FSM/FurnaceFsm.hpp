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

#include "EventQueueManager.hpp"
#include "Events.hpp"
#include "../Furnace/Furnace.hpp"
#include "../Furnace/State.hpp"
#include "../Log/LogService.hpp"

#include <etl/fsm.h>

namespace HeatTreatFurnace
{
namespace FSM
{

/**
 * @brief Main FSM class for furnace state management
 *
 * Derives from etl::fsm to handle state transitions automatically.
 * Overrides receive() to queue messages, then processes them via
 * etl::fsm::receive() during ProcessQueue().
 */
class FurnaceFsm : public etl::fsm
{
public:
    FurnaceFsm(Furnace::FurnaceState& aFurnaceState, Log::LogService& aLogger);

    /**
     * @brief Overridden receive to queue messages instead of processing immediately
     * @param aMsg Message to queue for later processing
     */
    void receive(etl::imessage const& aMsg) override;

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

    /**
     * @brief Get the current state ID
     * @return Current state identifier (mapped to Furnace::StateId)
     */
    Furnace::StateId GetCurrentState() const noexcept;

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
    uint32_t GetOverflowCount() const noexcept;

private:
    EventQueueManager myQueueManager;
    Furnace::FurnaceState& myFurnaceState;
    Log::LogService& myLogger;
};

}  // namespace FSM
}  // namespace HeatTreatFurnace
