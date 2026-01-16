#include "Furnace/EventQueueManager.hpp"
#include "Log/LogService.hpp"


namespace HeatTreatFurnace::Furnace
{
    EventQueueManager::EventQueueManager(Log::LogService& aLogger)
        : myQueue(),
          myMutex(),
          mySequence(0U),
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

    bool EventQueueManager::PrivHandleOverflow(EventPriority aPriority)
    {
        myOverflowCount++;

        // Log the overflow
        // TODO: Use actual logging once integrated

        // Route to ERROR if Critical or Furnace priority overflows
        bool shouldRouteToError = (aPriority == EventPriority::Critical || aPriority == EventPriority::Furnace);
        if (shouldRouteToError)
        {
            EvtError evt = EvtError(Error::EventQueueOverflow, Domain::Furnace, "EventQueueManager overflowed");
            Post(evt);
        }

        return shouldRouteToError;
    }
} // namespace FSM

