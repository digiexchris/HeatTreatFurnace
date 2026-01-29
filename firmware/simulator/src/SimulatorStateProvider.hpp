#pragma once

#include "Communication/IStateProvider.hpp"
#include "Furnace/FurnaceFsm.hpp"
#include "ThermalSimulator.hpp"
#include <chrono>

namespace Simulator
{
    /**
     * @brief State provider implementation for simulator
     *
     * Maps FSM state to FlatBuffers state data, incorporating
     * temperature readings from ThermalSimulator and time scaling.
     */
    class SimulatorStateProvider : public HeatTreatFurnace::Communication::IStateProvider
    {
    public:
        SimulatorStateProvider(
            HeatTreatFurnace::Furnace::FurnaceFsm& aFsm,
            ThermalSimulator& aThermal);

        ~SimulatorStateProvider() override = default;

        HeatTreatFurnace::Communication::FurnaceStateData GetCurrentState() const override;

        /**
         * @brief Update simulated time by delta
         * @param aDeltaMs Real-time milliseconds elapsed (will be scaled by time scale)
         */
        void UpdateTime(uint32_t aDeltaMs);

        /**
         * @brief Set the start time for current program
         */
        void SetProgramStartTime();

        /**
         * @brief Clear program timing info
         */
        void ClearProgramTiming();

        /**
         * @brief Set the expected program end time
         * @param aDurationMs Duration in simulated milliseconds
         */
        void SetProgramDuration(int64_t aDurationMs);

        /**
         * @brief Set error message for error state
         */
        void SetErrorMessage(const char* aMessage);

        /**
         * @brief Clear error message
         */
        void ClearError();

    private:
        ::Furnace::FurnaceMode PrivMapStateToMode(HeatTreatFurnace::Furnace::StateId aStateId) const;
        ::Furnace::ProfileSubState PrivMapStateToProfileSubState(HeatTreatFurnace::Furnace::StateId aStateId) const;
        ::Furnace::ManualSubState PrivMapStateToManualSubState(HeatTreatFurnace::Furnace::StateId aStateId) const;

        HeatTreatFurnace::Furnace::FurnaceFsm& myFsm;
        ThermalSimulator& myThermal;
        int64_t mySimulatedTimeMs;
        int64_t myProgramStartMs;
        int64_t myProgramEndMs;
        etl::string<128> myErrorMessage;
    };
} // namespace Simulator
