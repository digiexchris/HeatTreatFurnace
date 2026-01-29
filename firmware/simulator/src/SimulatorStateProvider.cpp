#include "SimulatorStateProvider.hpp"

namespace Simulator
{
    SimulatorStateProvider::SimulatorStateProvider(
        HeatTreatFurnace::Furnace::FurnaceFsm& aFsm,
        ThermalSimulator& aThermal)
        : myFsm(aFsm)
        , myThermal(aThermal)
        , mySimulatedTimeMs(0)
        , myProgramStartMs(0)
        , myProgramEndMs(0)
    {
    }

    HeatTreatFurnace::Communication::FurnaceStateData SimulatorStateProvider::GetCurrentState() const
    {
        HeatTreatFurnace::Communication::FurnaceStateData state;

        auto stateId = myFsm.GetCurrentState();

        state.mode = PrivMapStateToMode(stateId);
        state.profileState = PrivMapStateToProfileSubState(stateId);
        state.manualState = PrivMapStateToManualSubState(stateId);

        // Temperature data from thermal simulator
        state.kilnTemp = myThermal.GetKilnTemp();
        state.setTemp = myThermal.GetTargetTemp();
        state.envTemp = myThermal.GetEnvTemp();
        state.caseTemp = myThermal.GetCaseTemp();
        state.heatPercent = myThermal.GetHeatPercent();
        state.tempChange = myThermal.GetTempChangeRate();

        // Program info from FSM
        if (myFsm.IsProfileSet())
        {
            auto profile = myFsm.GetCurrentProfile();
            state.programName = profile.name;

            // Build step description
            if (profile.currentSegment < profile.segments.size())
            {
                auto& seg = profile.segments[profile.currentSegment];
                char stepBuf[64];
                snprintf(stepBuf, sizeof(stepBuf), "Segment %u: %.0f°C",
                         profile.currentSegment + 1, seg.target);
                state.step = stepBuf;
            }
        }

        // Timing info
        state.progStartMs = myProgramStartMs;
        state.progEndMs = myProgramEndMs;
        state.currTimeMs = mySimulatedTimeMs;

        // Error message
        state.errorMessage = myErrorMessage;

        // Simulator flags
        state.isSimulator = true;
        state.timeScale = myThermal.GetTimeScale();

        return state;
    }

    void SimulatorStateProvider::UpdateTime(uint32_t aDeltaMs)
    {
        float timeScale = myThermal.GetTimeScale();
        mySimulatedTimeMs += static_cast<int64_t>(static_cast<float>(aDeltaMs) * timeScale);
    }

    void SimulatorStateProvider::SetProgramStartTime()
    {
        myProgramStartMs = mySimulatedTimeMs;
    }

    void SimulatorStateProvider::ClearProgramTiming()
    {
        myProgramStartMs = 0;
        myProgramEndMs = 0;
    }

    void SimulatorStateProvider::SetProgramDuration(int64_t aDurationMs)
    {
        myProgramEndMs = myProgramStartMs + aDurationMs;
    }

    void SimulatorStateProvider::SetErrorMessage(const char* aMessage)
    {
        myErrorMessage = aMessage;
    }

    void SimulatorStateProvider::ClearError()
    {
        myErrorMessage.clear();
    }

    ::Furnace::FurnaceMode SimulatorStateProvider::PrivMapStateToMode(
        HeatTreatFurnace::Furnace::StateId aStateId) const
    {
        using StateId = HeatTreatFurnace::Furnace::StateId;

        switch (aStateId)
        {
        case StateId::OFF:
            return ::Furnace::FurnaceMode_Off;
        case StateId::ERROR:
            return ::Furnace::FurnaceMode_Error;
        case StateId::PROFILE:
        case StateId::PROFILE_LOADED:
        case StateId::PROFILE_RUNNING:
        case StateId::PROFILE_COMPLETED:
        case StateId::PROFILE_STOPPED:
            return ::Furnace::FurnaceMode_Profile;
        case StateId::MANUAL:
        case StateId::MANUAL_OFF:
        case StateId::MANUAL_ON:
            return ::Furnace::FurnaceMode_Manual;
        default:
            return ::Furnace::FurnaceMode_Off;
        }
    }

    ::Furnace::ProfileSubState SimulatorStateProvider::PrivMapStateToProfileSubState(
        HeatTreatFurnace::Furnace::StateId aStateId) const
    {
        using StateId = HeatTreatFurnace::Furnace::StateId;

        switch (aStateId)
        {
        case StateId::PROFILE_LOADED:
            return ::Furnace::ProfileSubState_Loaded;
        case StateId::PROFILE_RUNNING:
            return ::Furnace::ProfileSubState_Running;
        case StateId::PROFILE_STOPPED:
            return ::Furnace::ProfileSubState_Stopped;
        case StateId::PROFILE_COMPLETED:
            return ::Furnace::ProfileSubState_Completed;
        default:
            return ::Furnace::ProfileSubState_None;
        }
    }

    ::Furnace::ManualSubState SimulatorStateProvider::PrivMapStateToManualSubState(
        HeatTreatFurnace::Furnace::StateId aStateId) const
    {
        using StateId = HeatTreatFurnace::Furnace::StateId;

        switch (aStateId)
        {
        case StateId::MANUAL_ON:
            return ::Furnace::ManualSubState_On;
        case StateId::MANUAL_OFF:
        case StateId::MANUAL:
            return ::Furnace::ManualSubState_Off;
        default:
            return ::Furnace::ManualSubState_Off;
        }
    }
} // namespace Simulator
