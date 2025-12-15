// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Chris Sutton

#include "FsmTestFixture.hpp"

namespace HeatTreatFurnace::Test
{
    TEST_CASE("PROFILE_TEMP_OVERRIDE: EvtResume transitions to RUNNING", "[fsm][profile_temp_override]")
    {
        FsmTestFixture fixture;
        Profile profile;

        // Transition to PROFILE_TEMP_OVERRIDE
        EvtLoadProfile loadEvt(profile);
        fixture.fsm.Post(loadEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        EvtStart startEvt;
        fixture.fsm.Post(startEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        EvtSetManualTemp manualEvt(150.0f);
        fixture.fsm.Post(manualEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        // Then resume
        EvtResume evt;
        fixture.fsm.Post(evt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        REQUIRE(fixture.fsm.GetCurrentState() == StateId::RUNNING);
    }

    TEST_CASE("PROFILE_TEMP_OVERRIDE: EvtSetManualTemp stays in PROFILE_TEMP_OVERRIDE", "[fsm][profile_temp_override]")
    {
        FsmTestFixture fixture;
        Profile profile;

        // Transition to PROFILE_TEMP_OVERRIDE
        EvtLoadProfile loadEvt(profile);
        fixture.fsm.Post(loadEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        EvtStart startEvt;
        fixture.fsm.Post(startEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        EvtSetManualTemp manualEvt1(150.0f);
        fixture.fsm.Post(manualEvt1, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        // Then update manual temp
        EvtSetManualTemp manualEvt2(200.0f);
        fixture.fsm.Post(manualEvt2, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_TEMP_OVERRIDE);
    }

    TEST_CASE("PROFILE_TEMP_OVERRIDE: EvtError transitions to ERROR", "[fsm][profile_temp_override]")
    {
        FsmTestFixture fixture;
        Profile profile;

        // Transition to PROFILE_TEMP_OVERRIDE
        EvtLoadProfile loadEvt(profile);
        fixture.fsm.Post(loadEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        EvtStart startEvt;
        fixture.fsm.Post(startEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        EvtSetManualTemp manualEvt(150.0f);
        fixture.fsm.Post(manualEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        // Then error
        EvtError evt(Error::SensorFailure, Domain::Furnace, "Test error");
        fixture.fsm.Post(evt, EventPriority::Critical);
        fixture.fsm.ProcessQueue();

        REQUIRE(fixture.fsm.GetCurrentState() == StateId::ERROR);
    }

}  // namespace HeatTreatFurnace::Test
