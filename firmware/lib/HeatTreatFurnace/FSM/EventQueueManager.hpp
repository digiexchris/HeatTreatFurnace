// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Chris Sutton

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

class EventQueueManager
{
public:
    explicit EventQueueManager(Log::LogService& aLogger);

    bool Post(etl::imessage const& aMsg, EventPriority aPriority);

    template<typename Handler>
    void DrainQueue(Handler&& aHandler);

    uint32_t GetOverflowCount() const noexcept;
    void ResetOverflowCount() noexcept;

private:
    EventPriorityQueue myQueue;
    std::mutex myMutex;
    uint32_t mySequence{0U};
    uint32_t myOverflowCount{0U};
    Log::LogService& myLogger;

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
