// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Chris Sutton

#include "FsmTestFixture.hpp"

namespace HeatTreatFurnace::Test
{
    TEST_CASE("COMPLETED: EvtClearProgram transitions to IDLE", "[fsm][completed]")
    {
        FsmTestFixture fixture;
        Profile profile;

        // Transition to COMPLETED
        EvtLoadProfile loadEvt(profile);
        fixture.fsm.Post(loadEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        EvtStart startEvt;
        fixture.fsm.Post(startEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        EvtComplete completeEvt;
        fixture.fsm.Post(completeEvt, EventPriority::Furnace);
        fixture.fsm.ProcessQueue();

        // Then clear program
        EvtClearProgram evt;
        fixture.fsm.Post(evt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        REQUIRE(fixture.fsm.GetCurrentState() == StateId::IDLE);
    }

    TEST_CASE("COMPLETED: EvtLoadProfile transitions to LOADED", "[fsm][completed]")
    {
        FsmTestFixture fixture;
        Profile profile;

        // Transition to COMPLETED
        EvtLoadProfile loadEvt1(profile);
        fixture.fsm.Post(loadEvt1, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        EvtStart startEvt;
        fixture.fsm.Post(startEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        EvtComplete completeEvt;
        fixture.fsm.Post(completeEvt, EventPriority::Furnace);
        fixture.fsm.ProcessQueue();

        // Then load new profile
        EvtLoadProfile loadEvt2(profile);
        fixture.fsm.Post(loadEvt2, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        REQUIRE(fixture.fsm.GetCurrentState() == StateId::LOADED);
    }

    TEST_CASE("COMPLETED: EvtSetManualTemp transitions to MANUAL_TEMP", "[fsm][completed]")
    {
        FsmTestFixture fixture;
        Profile profile;

        // Transition to COMPLETED
        EvtLoadProfile loadEvt(profile);
        fixture.fsm.Post(loadEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        EvtStart startEvt;
        fixture.fsm.Post(startEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        EvtComplete completeEvt;
        fixture.fsm.Post(completeEvt, EventPriority::Furnace);
        fixture.fsm.ProcessQueue();

        // Then set manual temp
        EvtSetManualTemp evt(100.0f);
        fixture.fsm.Post(evt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        REQUIRE(fixture.fsm.GetCurrentState() == StateId::MANUAL_TEMP);
    }

    TEST_CASE("COMPLETED: EvtError transitions to ERROR", "[fsm][completed]")
    {
        FsmTestFixture fixture;
        Profile profile;

        // Transition to COMPLETED
        EvtLoadProfile loadEvt(profile);
        fixture.fsm.Post(loadEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        EvtStart startEvt;
        fixture.fsm.Post(startEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        EvtComplete completeEvt;
        fixture.fsm.Post(completeEvt, EventPriority::Furnace);
        fixture.fsm.ProcessQueue();

        // Then error
        EvtError evt(Error::SensorFailure, Domain::Furnace, "Test error");
        fixture.fsm.Post(evt, EventPriority::Critical);
        fixture.fsm.ProcessQueue();

        REQUIRE(fixture.fsm.GetCurrentState() == StateId::ERROR);
    }

}  // namespace HeatTreatFurnace::Test
