// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Chris Sutton

#pragma once

#include <cstdint>

namespace HeatTreatFurnace
{
namespace FSM
{

enum class Error : uint8_t
{
    Unknown,
    SafetyInterlock,
    SensorFailure,
    ControllerFailure,
    ProfileInvalid,
    TemperatureOutOfBounds
};

enum class Domain : uint8_t
{
    UI,
    Furnace,
    StateMachine
};

}  // namespace FSM
}  // namespace HeatTreatFurnace
