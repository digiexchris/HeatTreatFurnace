// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Chris Sutton

#include "FsmTestFixture.hpp"

namespace HeatTreatFurnace::Test
{
    TEST_CASE("CANCELLED: EvtLoadProfile transitions to LOADED", "[fsm][cancelled]")
    {
        FsmTestFixture fixture;
        Profile profile;
        Profile profile2;

        // Transition to CANCELLED
        EvtLoadProfile loadEvt1(profile);
        fixture.fsm.Post(loadEvt1, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        EvtStart startEvt;
        fixture.fsm.Post(startEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        EvtCancel cancelEvt;
        fixture.fsm.Post(cancelEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        // Then load new profile
        EvtLoadProfile loadEvt2(profile2);
        fixture.fsm.Post(loadEvt2, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        REQUIRE(fixture.fsm.GetCurrentState() == StateId::LOADED);
    }

    TEST_CASE("CANCELLED: EvtClearProgram transitions to IDLE", "[fsm][cancelled]")
    {
        FsmTestFixture fixture;
        Profile profile;

        // Transition to CANCELLED
        EvtLoadProfile loadEvt(profile);
        fixture.fsm.Post(loadEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        EvtStart startEvt;
        fixture.fsm.Post(startEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        EvtCancel cancelEvt;
        fixture.fsm.Post(cancelEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        // Then clear program
        EvtClearProgram evt;
        fixture.fsm.Post(evt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        REQUIRE(fixture.fsm.GetCurrentState() == StateId::IDLE);
    }

    TEST_CASE("CANCELLED: EvtError transitions to ERROR", "[fsm][cancelled]")
    {
        FsmTestFixture fixture;
        Profile profile;

        // Transition to CANCELLED
        EvtLoadProfile loadEvt(profile);
        fixture.fsm.Post(loadEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        EvtStart startEvt;
        fixture.fsm.Post(startEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        EvtCancel cancelEvt;
        fixture.fsm.Post(cancelEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        // Then error
        EvtError evt(Error::SensorFailure, Domain::Furnace, "Test error");
        fixture.fsm.Post(evt, EventPriority::Critical);
        fixture.fsm.ProcessQueue();

        REQUIRE(fixture.fsm.GetCurrentState() == StateId::ERROR);
    }

}  // namespace HeatTreatFurnace::Test
