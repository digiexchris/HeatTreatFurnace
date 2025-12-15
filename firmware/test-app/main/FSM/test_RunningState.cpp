// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Chris Sutton

#include "FsmTestFixture.hpp"

namespace HeatTreatFurnace::Test
{
    TEST_CASE("RUNNING: EvtPause transitions to PAUSED", "[fsm][running]")
    {
        FsmTestFixture fixture;
        Profile profile;

        // Transition to RUNNING
        EvtLoadProfile loadEvt(profile);
        fixture.fsm.Post(loadEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        EvtStart startEvt;
        fixture.fsm.Post(startEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        // Then pause
        EvtPause evt;
        fixture.fsm.Post(evt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        REQUIRE(fixture.fsm.GetCurrentState() == StateId::PAUSED);
    }

    TEST_CASE("RUNNING: EvtComplete transitions to COMPLETED", "[fsm][running]")
    {
        FsmTestFixture fixture;
        Profile profile;

        // Transition to RUNNING
        EvtLoadProfile loadEvt(profile);
        fixture.fsm.Post(loadEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        EvtStart startEvt;
        fixture.fsm.Post(startEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        // Then complete
        EvtComplete evt;
        fixture.fsm.Post(evt, EventPriority::Furnace);
        fixture.fsm.ProcessQueue();

        REQUIRE(fixture.fsm.GetCurrentState() == StateId::COMPLETED);
    }

    TEST_CASE("RUNNING: EvtCancel transitions to CANCELLED", "[fsm][running]")
    {
        FsmTestFixture fixture;
        Profile profile;

        // Transition to RUNNING
        EvtLoadProfile loadEvt(profile);
        fixture.fsm.Post(loadEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        EvtStart startEvt;
        fixture.fsm.Post(startEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        // Then cancel
        EvtCancel evt;
        fixture.fsm.Post(evt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        REQUIRE(fixture.fsm.GetCurrentState() == StateId::CANCELLED);
    }

    TEST_CASE("RUNNING: EvtSetManualTemp transitions to PROFILE_TEMP_OVERRIDE", "[fsm][running]")
    {
        FsmTestFixture fixture;
        Profile profile;

        // Transition to RUNNING
        EvtLoadProfile loadEvt(profile);
        fixture.fsm.Post(loadEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        EvtStart startEvt;
        fixture.fsm.Post(startEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        // Then set manual temp
        EvtSetManualTemp evt(150.0f);
        fixture.fsm.Post(evt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_TEMP_OVERRIDE);
    }

    TEST_CASE("RUNNING: EvtError transitions to ERROR", "[fsm][running]")
    {
        FsmTestFixture fixture;
        Profile profile;

        // Transition to RUNNING
        EvtLoadProfile loadEvt(profile);
        fixture.fsm.Post(loadEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        EvtStart startEvt;
        fixture.fsm.Post(startEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        // Then error
        EvtError evt(Error::SensorFailure, Domain::Furnace, "Test error");
        fixture.fsm.Post(evt, EventPriority::Critical);
        fixture.fsm.ProcessQueue();

        REQUIRE(fixture.fsm.GetCurrentState() == StateId::ERROR);
    }

}  // namespace HeatTreatFurnace::Test
