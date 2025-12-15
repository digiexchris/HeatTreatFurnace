// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Chris Sutton

#include "FsmTestFixture.hpp"

namespace HeatTreatFurnace::Test
{
    TEST_CASE("LOADED: EvtStart transitions to RUNNING", "[fsm][loaded]")
    {
        FsmTestFixture fixture;
        Profile profile;

        // First transition to LOADED
        EvtLoadProfile loadEvt(profile);
        fixture.fsm.Post(loadEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        // Then test EvtStart
        EvtStart evt;
        fixture.fsm.Post(evt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        REQUIRE(fixture.fsm.GetCurrentState() == StateId::RUNNING);
    }

    TEST_CASE("LOADED: EvtLoadProfile stays in LOADED", "[fsm][loaded]")
    {
        FsmTestFixture fixture;
        Profile profile;
        Profile profile2;

        // First transition to LOADED
        EvtLoadProfile loadEvt1(profile);
        fixture.fsm.Post(loadEvt1, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        // Load another profile
        EvtLoadProfile loadEvt2(profile2);
        fixture.fsm.Post(loadEvt2, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        REQUIRE(fixture.fsm.GetCurrentState() == StateId::LOADED);
    }

    TEST_CASE("LOADED: EvtClearProgram transitions to IDLE", "[fsm][loaded]")
    {
        FsmTestFixture fixture;
        Profile profile;

        // First transition to LOADED
        EvtLoadProfile loadEvt(profile);
        fixture.fsm.Post(loadEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        // Then clear program
        EvtClearProgram evt;
        fixture.fsm.Post(evt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        REQUIRE(fixture.fsm.GetCurrentState() == StateId::IDLE);
    }

    TEST_CASE("LOADED: EvtSetManualTemp transitions to MANUAL_TEMP", "[fsm][loaded]")
    {
        FsmTestFixture fixture;
        Profile profile;

        // First transition to LOADED
        EvtLoadProfile loadEvt(profile);
        fixture.fsm.Post(loadEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        // Then set manual temp
        EvtSetManualTemp evt(100.0f);
        fixture.fsm.Post(evt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        REQUIRE(fixture.fsm.GetCurrentState() == StateId::MANUAL_TEMP);
    }

    TEST_CASE("LOADED: EvtError transitions to ERROR", "[fsm][loaded]")
    {
        FsmTestFixture fixture;
        Profile profile;

        // First transition to LOADED
        EvtLoadProfile loadEvt(profile);
        fixture.fsm.Post(loadEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        // Then error
        EvtError evt(Error::SafetyInterlock, Domain::Furnace, "Test error");
        fixture.fsm.Post(evt, EventPriority::Critical);
        fixture.fsm.ProcessQueue();

        REQUIRE(fixture.fsm.GetCurrentState() == StateId::ERROR);
    }

}  // namespace HeatTreatFurnace::Test
