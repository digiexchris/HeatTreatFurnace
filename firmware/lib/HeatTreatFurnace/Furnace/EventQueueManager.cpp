#include "Furnace/EventQueueManager.hpp"
#include "Log/LogService.hpp"


namespace HeatTreatFurnace::Furnace
{
    EventQueueManager::EventQueueManager(Log::LogService& aLogger)
        : myQueue(),
          myMutex(),
          myOverflowCount(0U),
          myLogger(aLogger)
    {
    }

    uint32_t EventQueueManager::GetOverflowCount() const noexcept
    {
        return myOverflowCount;
    }

    void EventQueueManager::ResetOverflowCount() noexcept
    {
        myOverflowCount = 0U;
    }

    bool EventQueueManager::PrivHandleOverflow(EventId andEvtId)
    {
        myOverflowCount++;
        myLogger.Log(Log::LogLevel::Warn, "QUEUE", "Queue full inserting {}", EventIdToString(andEvtId));
        return false;
    }
} // namespace FSM

