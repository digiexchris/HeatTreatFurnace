#pragma once

#include "Communication/Generated/furnace_generated.h"
#include "Furnace/StateId.hpp"
#include <etl/string.h>

namespace HeatTreatFurnace::Communication
{
    /**
     * @brief Struct holding current furnace state for FlatBuffers State message
     */
    struct FurnaceStateData
    {
        ::Furnace::FurnaceMode mode = ::Furnace::FurnaceMode_Off;
        ::Furnace::ProfileSubState profileState = ::Furnace::ProfileSubState_None;
        ::Furnace::ManualSubState manualState = ::Furnace::ManualSubState_Off;
        etl::string<64> programName;
        float kilnTemp = 0.0f;
        float setTemp = 0.0f;
        float envTemp = 0.0f;
        float caseTemp = 0.0f;
        uint8_t heatPercent = 0;
        float tempChange = 0.0f;
        etl::string<64> step;
        int64_t progStartMs = 0;
        int64_t progEndMs = 0;
        int64_t currTimeMs = 0;
        etl::string<128> errorMessage;
        bool isSimulator = false;
        float timeScale = 1.0f;
    };

    /**
     * @brief Abstract interface for providing current furnace state
     *
     * Used by message handlers to build State response messages.
     * Implementations:
     * - Real implementation queries FSM and sensor data
     * - Mock implementation returns hardcoded placeholder values for tests
     */
    class IStateProvider
    {
    public:
        virtual ~IStateProvider() = default;

        /**
         * @brief Get current furnace state data
         * @return FurnaceStateData populated with current values
         */
        virtual FurnaceStateData GetCurrentState() const = 0;
    };
} // namespace HeatTreatFurnace::Communication
