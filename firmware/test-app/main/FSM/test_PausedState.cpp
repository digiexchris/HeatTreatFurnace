// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Chris Sutton

#include "FsmTestFixture.hpp"

namespace HeatTreatFurnace::Test
{
    TEST_CASE("PAUSED: EvtResume transitions to RUNNING", "[fsm][paused]")
    {
        FsmTestFixture fixture;
        Profile profile;

        // Transition to PAUSED
        EvtLoadProfile loadEvt(profile);
        fixture.fsm.Post(loadEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        EvtStart startEvt;
        fixture.fsm.Post(startEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        EvtPause pauseEvt;
        fixture.fsm.Post(pauseEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        // Then resume
        EvtResume evt;
        fixture.fsm.Post(evt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        REQUIRE(fixture.fsm.GetCurrentState() == StateId::RUNNING);
    }

    TEST_CASE("PAUSED: EvtCancel transitions to CANCELLED", "[fsm][paused]")
    {
        FsmTestFixture fixture;
        Profile profile;

        // Transition to PAUSED
        EvtLoadProfile loadEvt(profile);
        fixture.fsm.Post(loadEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        EvtStart startEvt;
        fixture.fsm.Post(startEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        EvtPause pauseEvt;
        fixture.fsm.Post(pauseEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        // Then cancel
        EvtCancel evt;
        fixture.fsm.Post(evt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        REQUIRE(fixture.fsm.GetCurrentState() == StateId::CANCELLED);
    }

    TEST_CASE("PAUSED: EvtError transitions to ERROR", "[fsm][paused]")
    {
        FsmTestFixture fixture;
        Profile profile;

        // Transition to PAUSED
        EvtLoadProfile loadEvt(profile);
        fixture.fsm.Post(loadEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        EvtStart startEvt;
        fixture.fsm.Post(startEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        EvtPause pauseEvt;
        fixture.fsm.Post(pauseEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        // Then error
        EvtError evt(Error::SensorFailure, Domain::Furnace, "Test error");
        fixture.fsm.Post(evt, EventPriority::Critical);
        fixture.fsm.ProcessQueue();

        REQUIRE(fixture.fsm.GetCurrentState() == StateId::ERROR);
    }

}  // namespace HeatTreatFurnace::Test
