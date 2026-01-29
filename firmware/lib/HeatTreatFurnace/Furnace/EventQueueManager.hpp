#pragma once

#include "Log/LogService.hpp"

#include <etl/message.h>
#include <mutex>

#include "EventQueue.hpp"

namespace HeatTreatFurnace::Furnace
{
    /**
     * @brief Thread-safe manager for the FSM event FIFO queue
     *
     * Manages posting events and draining them for processing in FIFO order.
     * Handles overflow conditions by routing to ERROR state.
     */
    class EventQueueManager
    {
    public:
        explicit EventQueueManager(Log::LogService& aLogger);

        /**
         * @brief Post an event to the queue
         * @param aMsg The event message to post
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
                success = PrivHandleOverflow();
            }

            return success;
        }

        /**
         * @brief Drain and process all events from the queue in FIFO order
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
                    packet = myQueue.front();
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
         * @brief Flush all events from the queue without processing
         *
         * Clears the queue and releases all packets back to the pool.
         * Used when an error event is posted to discard pending events.
         */
        void Flush()
        {
            std::lock_guard<std::mutex> lock(myMutex);
            while (!myQueue.empty())
            {
                MessagePacket* packet = myQueue.front();
                myQueue.pop();
                myEventPool.release(packet);
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
        EventQueue myQueue;
        etl::pool<MessagePacket, 48U> myEventPool; //statically allocated memory for creating events on
        std::mutex myMutex;
        uint32_t myOverflowCount{0U};
        Log::LogService& myLogger;

        /**
         * @brief Handle queue overflow
         * @return false (overflow condition)
         */
        bool PrivHandleOverflow();
    };
} // namespace HeatTreatFurnace::FSM

