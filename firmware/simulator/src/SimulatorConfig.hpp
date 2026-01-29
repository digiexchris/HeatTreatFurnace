#pragma once

#include <cstdint>
#include <cstddef>

namespace Simulator::Config
{
    // WebSocket server configuration
    constexpr uint16_t DEFAULT_PORT = 5173;
    constexpr const char* WS_PATH = "/ws";

    // Thermal simulation parameters
    constexpr float HEATER_POWER = 0.5f; // degC/sec at full power
    constexpr float COOLING_COEFFICIENT = 0.0001f; // Newton's cooling coefficient
    constexpr float THERMAL_MASS = 100.0f; // Thermal inertia
    constexpr float AMBIENT_TEMP = 20.0f; // degC
    constexpr float CASE_TEMP_FACTOR = 0.3f; // Case temp is this fraction of kiln temp above ambient

    // PID tuning parameters
    constexpr float PID_KP = 2.0f;
    constexpr float PID_KI = 0.01f;
    constexpr float PID_KD = 0.5f;
    constexpr float PID_OUTPUT_MIN = 0.0f;
    constexpr float PID_OUTPUT_MAX = 100.0f;

    // Timing configuration
    constexpr uint32_t TICK_INTERVAL_MS = 100; // Main loop tick interval
    constexpr uint32_t STATE_BROADCAST_INTERVAL_MS = 500; // How often to broadcast state
    constexpr float MIN_TIME_SCALE = 1.0f;
    constexpr float MAX_TIME_SCALE = 100.0f;
    constexpr float DEFAULT_TIME_SCALE = 1.0f;

    // Program storage
    constexpr const char* PROGRAMS_DIR = "programs/";
    constexpr size_t MAX_PROGRAM_NAME_LENGTH = 64;
}

