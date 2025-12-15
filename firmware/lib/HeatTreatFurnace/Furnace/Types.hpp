// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Chris Sutton

/**
 * @file Types.hpp
 * @brief Common type definitions for the FSM implementation
 *
 * This file defines error codes and domain identifiers used throughout the FSM.
 * These types are used for error handling, logging, and event routing.
 */

#pragma once

#include <cstdint>

namespace HeatTreatFurnace::Furnace
{
    /// Maximum size in bytes for any event message stored in the queue
    constexpr std::size_t MaxMessageSize = 128U;

    /**
     * @brief Priority levels for events (lower enum value = higher priority)
     */
    enum class EventPriority : uint8_t
    {
        Critical = 0U, ///< Highest priority (errors, safety)
        Furnace = 1U, ///< Medium priority (control loop, state changes)
        UI = 2U ///< Lowest priority (user interface)
    };

    /**
     * @brief Error codes for FSM error handling
     *
     * Used in EvtError events to indicate the type of error that occurred.
     */
    enum class Error : uint8_t
    {
        Unknown,
        SafetyInterlock,
        SensorFailure,
        ControllerFailure,
        ProfileInvalid,
        TemperatureOutOfBounds
    };

    /**
     * @brief Domain indicating the origin of an error or event
     *
     * Used to categorize events by their source subsystem for logging and routing.
     */
    enum class Domain : uint8_t
    {
        UI,
        Furnace,
        StateMachine
    };
} // namespace HeatTreatFurnace::FSM

