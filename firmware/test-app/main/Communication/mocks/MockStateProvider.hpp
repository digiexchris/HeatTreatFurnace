#pragma once

#include "Communication/IStateProvider.hpp"

namespace HeatTreatFurnace::Test
{
    /**
     * @brief Mock state provider returning hardcoded placeholder values for tests
     */
    class MockStateProvider : public Communication::IStateProvider
    {
    public:
        Communication::FurnaceStateData GetCurrentState() const override
        {
            return myState;
        }

        /**
         * @brief Set the state to return
         */
        void SetState(const Communication::FurnaceStateData& aState)
        {
            myState = aState;
        }

        /**
         * @brief Get mutable reference to state for modification
         */
        Communication::FurnaceStateData& State()
        {
            return myState;
        }

        /**
         * @brief Reset to default placeholder values
         */
        void Reset()
        {
            myState = Communication::FurnaceStateData{};
            myState.mode = ::Furnace::FurnaceMode_Off;
            myState.profileState = ::Furnace::ProfileSubState_None;
            myState.manualState = ::Furnace::ManualSubState_Off;
            myState.kilnTemp = 25.0f;
            myState.setTemp = 0.0f;
            myState.envTemp = 22.0f;
            myState.caseTemp = 24.0f;
            myState.heatPercent = 0;
            myState.tempChange = 0.0f;
            myState.progStartMs = 0;
            myState.progEndMs = 0;
            myState.currTimeMs = 1000000; // Placeholder timestamp
            myState.isSimulator = false;
            myState.timeScale = 1.0f;
        }

    private:
        Communication::FurnaceStateData myState{};
    };
} // namespace HeatTreatFurnace::Test
