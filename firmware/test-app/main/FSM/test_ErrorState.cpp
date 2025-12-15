// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Chris Sutton

#include "FsmTestFixture.hpp"

namespace HeatTreatFurnace::Test
{
    TEST_CASE("ERROR: EvtReset transitions to IDLE", "[fsm][error]")
    {
        FsmTestFixture fixture;

        // Transition to ERROR
        EvtError evtErr(Error::SensorFailure, Domain::Furnace, "Test error");
        fixture.fsm.Post(evtErr, EventPriority::Critical);
        fixture.fsm.ProcessQueue();

        // Then reset
        EvtReset evt;
        fixture.fsm.Post(evt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        REQUIRE(fixture.fsm.GetCurrentState() == StateId::IDLE);
    }

    TEST_CASE("ERROR: EvtLoadProfile transitions to LOADED", "[fsm][error]")
    {
        FsmTestFixture fixture;
        Profile profile;

        // Transition to ERROR
        EvtError errorEvt(Error::SensorFailure, Domain::Furnace, "Test error");
        fixture.fsm.Post(errorEvt, EventPriority::Critical);
        fixture.fsm.ProcessQueue();

        // Then load profile
        EvtLoadProfile evt(profile);
        fixture.fsm.Post(evt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        REQUIRE(fixture.fsm.GetCurrentState() == StateId::LOADED);
    }

}  // namespace HeatTreatFurnace::Test
