// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Chris Sutton

#pragma once

#include "Types.hpp"

#include <etl/message.h>
#include <etl/string.h>

#include <cstdint>

namespace HeatTreatFurnace
{
namespace FSM
{

class Profile;
class Temperature;

enum EventId : etl::message_id_t
{
    EVENT_LOAD_PROFILE = 0,
    EVENT_START,
    EVENT_PAUSE,
    EVENT_RESUME,
    EVENT_CANCEL,
    EVENT_COMPLETE,
    EVENT_CLEAR_PROGRAM,
    EVENT_ERROR,
    EVENT_RESET,
    EVENT_SET_MANUAL_TEMP,
    EVENT_TICK
};

struct EvtLoadProfile : public etl::message<EVENT_LOAD_PROFILE>
{
    Profile const& profile;

    explicit EvtLoadProfile(Profile const& aProfile) : profile(aProfile) {}
};

struct EvtStart : public etl::message<EVENT_START> {};

struct EvtPause : public etl::message<EVENT_PAUSE> {};

struct EvtResume : public etl::message<EVENT_RESUME> {};

struct EvtCancel : public etl::message<EVENT_CANCEL> {};

struct EvtComplete : public etl::message<EVENT_COMPLETE> {};

struct EvtClearProgram : public etl::message<EVENT_CLEAR_PROGRAM> {};

struct EvtError : public etl::message<EVENT_ERROR>
{
    Error error;
    Domain domain;
    etl::string<96> msg;

    EvtError(Error anError, Domain aDomain, etl::string<96> const& aMessage)
        : error(anError), domain(aDomain), msg(aMessage) {}
};

struct EvtReset : public etl::message<EVENT_RESET> {};

struct EvtSetManualTemp : public etl::message<EVENT_SET_MANUAL_TEMP>
{
    Temperature const& temp;

    explicit EvtSetManualTemp(Temperature const& aTemp) : temp(aTemp) {}
};

struct EvtTick : public etl::message<EVENT_TICK> {};

}  // namespace FSM
}  // namespace HeatTreatFurnace
