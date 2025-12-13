// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Chris Sutton

#include "EventQueueManager.hpp"
#include "../Log/LogService.hpp"

namespace HeatTreatFurnace
{
namespace FSM
{

EventQueueManager::EventQueueManager(Log::LogService& aLogger)
    : myQueue(),
      myMutex(),
      mySequence(0U),
      myOverflowCount(0U),
      myLogger(aLogger)
{
}

bool EventQueueManager::Post(etl::imessage const& aMsg, EventPriority aPriority)
{
    std::lock_guard<std::mutex> lock(myMutex);

    bool success = false;

    if (!myQueue.full())
    {
        QueuedMsg queuedMsg(aPriority, mySequence++, aMsg);
        myQueue.push(queuedMsg);
        success = true;
    }
    else
    {
        bool shouldRouteToError = PrivHandleOverflow(aPriority);

        if (shouldRouteToError)
        {
            // TODO: Post EvtError to transition to ERROR state
        }
    }

    return success;
}

uint32_t EventQueueManager::GetOverflowCount() const noexcept
{
    return myOverflowCount;
}

void EventQueueManager::ResetOverflowCount() noexcept
{
    myOverflowCount = 0U;
}

bool EventQueueManager::PrivHandleOverflow(EventPriority aPriority)
{
    myOverflowCount++;

    // Log the overflow
    // TODO: Use actual logging once integrated

    // Route to ERROR if Critical or Furnace priority overflows
    bool shouldRouteToError = (aPriority == EventPriority::Critical || aPriority == EventPriority::Furnace);

    return shouldRouteToError;
}

}  // namespace FSM
}  // namespace HeatTreatFurnace
