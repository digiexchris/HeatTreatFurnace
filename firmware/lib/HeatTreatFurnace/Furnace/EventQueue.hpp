#pragma once

#include <etl/message.h>
#include <etl/message_packet.h>
#include <etl/priority_queue.h>
#include <etl/vector.h>

#include <cstdint>

#include "Events.hpp"
#include "Furnace/Types.hpp"

namespace HeatTreatFurnace::Furnace
{

    using MessagePacket = etl::message_packet
    <EvtModeOff, EvtModeProfile, EvtModeManual, EvtManualSetOff, EvtManualSetOn, EvtProfileLoad, EvtProfileAlreadyLoaded,
     EvtProfileStart, EvtProfileSetNextSegment, EvtProfileStop, EvtProfileClear, EvtProfileComplete, EvtManualSetTemp,
     EvtTick, EvtError
    >;
    /**
     * @brief Wrapper for queued messages with priority and sequence number
     *
     * Stores an event message along with its priority and a monotonic sequence number
     * to ensure FIFO ordering within the same priority level.
     */
    // struct QueuedMsg
    // {
    //     EventPriority priority; ///< Priority level of the message
    //     uint32_t seq; ///< Sequence number for FIFO ordering
    //     // etl::message_packet
    //     // <EvtModeOff, EvtModeProfile, EvtModeManual, EvtManualSetOff, EvtManualSetOn, EvtProfileLoad, EvtProfileAlreadyLoaded,
    //     //  EvtProfileStart, EvtProfileSetNextSegment, EvtProfileStop, EvtProfileClear, EvtManualSetTemp,
    //     //  EvtTick, EvtError
    //     // > packet;
    //     etl::imessage const& message;
    //
    //     QueuedMsg() : priority(EventPriority::UI), seq(0U), message()
    //     {
    //     }
    //
    //     QueuedMsg(EventPriority aPriority, uint32_t aSequence, etl::imessage const& aMsg)
    //         : priority(aPriority), seq(aSequence), message(aMsg)
    //     {
    //         int i = 1;
    //     }
    // };

    /**
     * @brief Comparator for priority queue ordering
     *
     * Implements min-heap behavior: lower priority enum value has higher precedence,
     * and within the same priority, earlier sequence number has higher precedence.
     */
    struct EvtPriorityCompare
    {
        bool operator()(const MessagePacket* lhs, const MessagePacket* rhs) const noexcept
        {
            auto const& a = static_cast<PriorityQueueEvent const&>(lhs->get());
            auto const& b = static_cast<PriorityQueueEvent const&>(rhs->get());

            if (a.priority != b.priority)
            {
                return static_cast<uint8_t>(a.priority) > static_cast<uint8_t>(b.priority);
            }
            return a.timestamp > b.timestamp;
        }
    };

    /// Priority queue type for storing and ordering events
    using EventPriorityQueue = etl::priority_queue<MessagePacket*, 48U, etl::vector<MessagePacket*, 48U>, EvtPriorityCompare>;
} // namespace HeatTreatFurnace::FSM

