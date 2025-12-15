// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Chris Sutton

#include "FsmTestFixture.hpp"

namespace HeatTreatFurnace::Test
{
    TEST_CASE("IDLE: EvtLoadProfile transitions to LOADED", "[fsm][idle]")
    {
        FsmTestFixture fixture;
        Profile profile;

        EvtLoadProfile evt(profile);
        fixture.fsm.Post(evt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        REQUIRE(fixture.fsm.GetCurrentState() == StateId::LOADED);
    }

    TEST_CASE("IDLE: EvtSetManualTemp transitions to MANUAL_TEMP", "[fsm][idle]")
    {
        FsmTestFixture fixture;

        EvtSetManualTemp evt(100.0f);
        fixture.fsm.Post(evt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        REQUIRE(fixture.fsm.GetCurrentState() == StateId::MANUAL_TEMP);
    }

    TEST_CASE("IDLE: EvtError transitions to ERROR", "[fsm][idle]")
    {
        FsmTestFixture fixture;

        EvtError evt(Error::SensorFailure, Domain::Furnace, "Test error");
        fixture.fsm.Post(evt, EventPriority::Critical);
        fixture.fsm.ProcessQueue();

        REQUIRE(fixture.fsm.GetCurrentState() == StateId::ERROR);
    }

}  // namespace HeatTreatFurnace::Test
