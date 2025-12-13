// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Chris Sutton

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

constexpr size_t MaxMessageSize = 128U;

enum class EventPriority : uint8_t
{
    Critical = 0U,
    Furnace = 1U,
    UI = 2U
};

struct QueuedMsg
{
    EventPriority priority;
    uint32_t seq;
    etl::message_packet<MaxMessageSize> packet;

    QueuedMsg() : priority(EventPriority::UI), seq(0U), packet() {}

    QueuedMsg(EventPriority aPriority, uint32_t aSequence, etl::imessage const& aMsg)
        : priority(aPriority), seq(aSequence), packet(aMsg) {}
};

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

using PQContainer = etl::vector<QueuedMsg, 48U>;
using EventPriorityQueue = etl::priority_queue<QueuedMsg, PQContainer, MsgCompare>;

}  // namespace FSM
}  // namespace HeatTreatFurnace
