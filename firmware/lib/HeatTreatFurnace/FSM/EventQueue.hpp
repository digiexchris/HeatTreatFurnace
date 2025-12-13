// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Chris Sutton

/**
 * @file EventQueue.hpp
 * @brief Priority queue infrastructure for FSM events
 *
 * Implements a bounded priority queue using ETL containers to store heterogeneous
 * event messages without dynamic allocation. Events are prioritized by EventPriority
 * (Critical > Furnace > UI) and within the same priority level, FIFO ordering is
 * maintained using sequence numbers.
 *
 * Total queue capacity: 48 entries (effectively 16 per priority class)
 */

#pragma once

#include <etl/message.h>
#include <etl/message_packet.h>
#include <etl/priority_queue.h>
#include <etl/vector.h>

#include <cstdint>

namespace HeatTreatFurnace
{
namespace FSM
{

/// Maximum size in bytes for any event message stored in the queue
constexpr size_t MaxMessageSize = 128U;

/**
 * @brief Priority levels for events (lower enum value = higher priority)
 */
enum class EventPriority : uint8_t
{
    Critical = 0U,  ///< Highest priority (errors, safety)
    Furnace = 1U,   ///< Medium priority (control loop, state changes)
    UI = 2U         ///< Lowest priority (user interface)
};

/**
 * @brief Wrapper for queued messages with priority and sequence number
 *
 * Stores an event message along with its priority and a monotonic sequence number
 * to ensure FIFO ordering within the same priority level.
 */
struct QueuedMsg
{
    EventPriority priority;  ///< Priority level of the message
    uint32_t seq;            ///< Sequence number for FIFO ordering
    etl::message_packet<MaxMessageSize> packet;  ///< Storage for the event message

    QueuedMsg() : priority(EventPriority::UI), seq(0U), packet() {}

    QueuedMsg(EventPriority aPriority, uint32_t aSequence, etl::imessage const& aMsg)
        : priority(aPriority), seq(aSequence), packet(aMsg) {}
};

/**
 * @brief Comparator for priority queue ordering
 *
 * Implements min-heap behavior: lower priority enum value has higher precedence,
 * and within the same priority, earlier sequence number has higher precedence.
 */
struct MsgCompare
{
    bool operator()(QueuedMsg const& a, QueuedMsg const& b) const noexcept
    {
        if (a.priority != b.priority)
        {
            return static_cast<uint8_t>(a.priority) > static_cast<uint8_t>(b.priority);
        }
        return a.seq > b.seq;
    }
};

/// Container type for the priority queue (48 total entries)
using PQContainer = etl::vector<QueuedMsg, 48U>;

/// Priority queue type for storing and ordering events
using EventPriorityQueue = etl::priority_queue<QueuedMsg, PQContainer, MsgCompare>;

}  // namespace FSM
}  // namespace HeatTreatFurnace
