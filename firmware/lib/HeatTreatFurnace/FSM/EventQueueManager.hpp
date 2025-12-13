// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Chris Sutton

/**
 * @file EventQueueManager.hpp
 * @brief Thread-safe event queue management for the FSM
 *
 * Provides a thread-safe wrapper around the priority queue for posting and draining
 * events. Handles queue overflow according to policy: drop-newest with logging and
 * counter increment. Critical/Furnace overflow triggers ERROR state transition.
 *
 * Usage:
 *   EventQueueManager queueMgr(logger);
 *   queueMgr.Post(event, EventPriority::Furnace);
 *   queueMgr.DrainQueue([&](etl::imessage& msg) { fsm.receive(msg); });
 *
 * Thread-safety: Uses std::mutex for synchronization in task/context code.
 * Not intended for ISR use.
 */

#pragma once

#include "EventQueue.hpp"
#include "Types.hpp"

#include <etl/imessage.h>

#include <cstdint>
#include <mutex>

namespace HeatTreatFurnace
{

namespace Log
{
class LogService;
}

namespace FSM
{

/**
 * @brief Thread-safe manager for the FSM event priority queue
 *
 * Manages posting events with priority levels and draining them for processing.
 * Tracks sequence numbers for FIFO ordering within priority levels and handles
 * overflow conditions according to the defined policy.
 */
class EventQueueManager
{
public:
    explicit EventQueueManager(Log::LogService& aLogger);

    /**
     * @brief Post an event to the queue with specified priority
     * @param aMsg The event message to post
     * @param aPriority Priority level for the event
     * @return true if posted successfully, false if queue is full
     */
    bool Post(etl::imessage const& aMsg, EventPriority aPriority);

    /**
     * @brief Drain and process all events from the queue in priority order
     * @param aHandler Callback to process each message (typically FSM receive)
     */
    template<typename Handler>
    void DrainQueue(Handler&& aHandler);

    /**
     * @brief Get the current overflow counter value
     * @return Number of events dropped due to queue overflow
     */
    uint32_t GetOverflowCount() const noexcept;

    /**
     * @brief Reset the overflow counter to zero
     */
    void ResetOverflowCount() noexcept;

private:
    EventPriorityQueue myQueue;
    std::mutex myMutex;
    uint32_t mySequence{0U};
    uint32_t myOverflowCount{0U};
    Log::LogService& myLogger;

    /**
     * @brief Handle queue overflow based on priority
     * @param aPriority Priority of the event that overflowed
     * @return true if should route to ERROR state (Critical or Furnace overflow)
     */
    bool PrivHandleOverflow(EventPriority aPriority);
};

template<typename Handler>
void EventQueueManager::DrainQueue(Handler&& aHandler)
{
    std::lock_guard<std::mutex> lock(myMutex);

    while (!myQueue.empty())
    {
        QueuedMsg msg = myQueue.top();
        myQueue.pop();

        aHandler(msg.packet.get());
    }
}

}  // namespace FSM
}  // namespace HeatTreatFurnace
