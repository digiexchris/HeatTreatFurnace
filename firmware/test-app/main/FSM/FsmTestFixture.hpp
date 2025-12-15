// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Chris Sutton

#pragma once

#include <catch2/catch_test_macros.hpp>
#include "etl_profile.h"
#include "Log/LogService.hpp"
#include "Log/LogBackend.hpp"
#include "FSM/FurnaceFsm.hpp"
#include "FSM/Events.hpp"
#include "FSM/States/IdleState.hpp"
#include "FSM/States/LoadedState.hpp"
#include "FSM/States/RunningState.hpp"
#include "FSM/States/PausedState.hpp"
#include "FSM/States/CompletedState.hpp"
#include "FSM/States/CancelledState.hpp"
#include "FSM/States/ErrorState.hpp"
#include "FSM/States/ManualTempState.hpp"
#include "FSM/States/ProfileTempOverrideState.hpp"
#include "Furnace/Furnace.hpp"
#include "mocks/LogBackend.hpp"

namespace HeatTreatFurnace::Test
{
    using namespace HeatTreatFurnace::Furnace;
    using namespace HeatTreatFurnace::Log;
    using namespace HeatTreatFurnace::FSM;



    class FsmTestFixture
    {
    public:
        FsmTestFixture() :
            mockBackend(LogLevel::None),
            logger(&mockBackend),
            furnaceState(),
            fsm(furnaceState, logger),
            idleState(logger),
            loadedState(logger),
            runningState(logger),
            pausedState(logger),
            completedState(logger),
            cancelledState(logger),
            errorState(logger),
            manualTempState(logger),
            profileTempOverrideState(logger)
        {
            etl::ifsm_state* states[] = {
                &idleState,
                &loadedState,
                &runningState,
                &pausedState,
                &completedState,
                &cancelledState,
                &errorState,
                &manualTempState,
                &profileTempOverrideState
            };

            fsm.set_states(states, 9);
            fsm.start();
        }

        MockLogBackend mockBackend;
        LogService logger;
        FurnaceState furnaceState;
        FurnaceFsm fsm;

        IdleState idleState;
        LoadedState loadedState;
        RunningState runningState;
        PausedState pausedState;
        CompletedState completedState;
        CancelledState cancelledState;
        ErrorState errorState;
        ManualTempState manualTempState;
        ProfileTempOverrideState profileTempOverrideState;
    };

}  // namespace HeatTreatFurnace::Test
