// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Chris Sutton

#include "FsmTestFixture.hpp"

namespace HeatTreatFurnace::Test
{
    TEST_CASE("MANUAL_TEMP: EvtResume transitions to IDLE when no profile loaded", "[fsm][manual_temp]")
    {
        FsmTestFixture fixture;

        // Transition to MANUAL_TEMP from IDLE
        EvtSetManualTemp manualEvt(100.0f);
        fixture.fsm.Post(manualEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        // Then resume
        EvtResume evt;
        fixture.fsm.Post(evt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        REQUIRE(fixture.fsm.GetCurrentState() == StateId::IDLE);
    }

    TEST_CASE("MANUAL_TEMP: EvtSetManualTemp stays in MANUAL_TEMP", "[fsm][manual_temp]")
    {
        FsmTestFixture fixture;

        // Transition to MANUAL_TEMP
        EvtSetManualTemp manualEvt1(100.0f);
        fixture.fsm.Post(manualEvt1, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        // Then update manual temp
        EvtSetManualTemp manualEvt2(150.0f);
        fixture.fsm.Post(manualEvt2, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        REQUIRE(fixture.fsm.GetCurrentState() == StateId::MANUAL_TEMP);
    }

    TEST_CASE("MANUAL_TEMP: EvtError transitions to ERROR", "[fsm][manual_temp]")
    {
        FsmTestFixture fixture;

        // Transition to MANUAL_TEMP
        EvtSetManualTemp manualEvt(100.0f);
        fixture.fsm.Post(manualEvt, EventPriority::UI);
        fixture.fsm.ProcessQueue();

        // Then error
        EvtError evt(Error::Unknown,  Domain::StateMachine, "aMessage");
        fixture.fsm.Post(evt, EventPriority::Critical);
        fixture.fsm.ProcessQueue();

        REQUIRE(fixture.fsm.GetCurrentState() == StateId::ERROR);
    }

}  // namespace HeatTreatFurnace::Test
