#pragma once

#include <etl/message.h>
#include <etl/message_packet.h>
#include <etl/queue.h>

#include <cstdint>

#include "Events.hpp"

namespace HeatTreatFurnace::Furnace
{

    using MessagePacket = etl::message_packet
    <EvtModeOff, EvtModeProfile, EvtModeManual, EvtManualSetOff, EvtManualSetOn, EvtProfileLoad, EvtProfileAlreadyLoaded,
     EvtProfileStart, EvtProfileSetNextSegment, EvtProfileStop, EvtProfileClear, EvtProfileComplete, EvtManualSetTemp,
     EvtTick, EvtError
    >;

    /// FIFO queue type for storing events
    using EventQueue = etl::queue<MessagePacket*, 48U>;
} // namespace HeatTreatFurnace::FSM

