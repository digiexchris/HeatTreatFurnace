#pragma once

#include "Types.hpp"
#include "Log/LogService.hpp"

#include <etl/message.h>
#include <mutex>

#include "EventQueue.hpp"

namespace HeatTreatFurnace::Furnace
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
        template <typename T>
        bool Post(T const& aMsg)
        {
            bool success = false;

            if (!myQueue.full())
            {
                std::lock_guard<std::mutex> lock(myMutex);
                MessagePacket* packet = myEventPool.allocate();
                new(packet) MessagePacket(aMsg);
                myQueue.push(packet);
                success = true;
            }
            else
            {
                success = PrivHandleOverflow(aMsg.priority);
            }

            return success;
        }

        /**
         * @brief Drain and process all events from the queue in priority order
         * @param aHandler Callback to process each message (typically FSM receive)
         */
        template <typename Handler>
        void DrainQueue(Handler&& aHandler)
        {
            while (!myQueue.empty())
            {
                MessagePacket* packet = nullptr;
                {
                    std::lock_guard<std::mutex> lock(myMutex);
                    packet = myQueue.top();
                    myQueue.pop();
                }

                assert(packet != nullptr); //TODO: there needs to be a watchdog that reboots if we're stuck on an assert too long
                aHandler(packet->get());

                {
                    std::lock_guard<std::mutex> lock(myMutex);
                    myEventPool.release(packet);
                }
            }
        }

        /**
         * @brief Get the current overflow counter value
         * @return Number of events dropped due to queue overflow
         */
        [[nodiscard]] uint32_t GetOverflowCount() const noexcept;

        /**
         * @brief Reset the overflow counter to zero
         */
        void ResetOverflowCount() noexcept;

    private:
        EventPriorityQueue myQueue;
        etl::pool<MessagePacket, 48U> myEventPool; //statically allocated memory for creating events on
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
} // namespace HeatTreatFurnace::FSM

