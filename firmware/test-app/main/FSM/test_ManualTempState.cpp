#include <doctest/doctest.h>
#include <doctest/trompeloeil.hpp>
#include "FsmTestFixture.hpp"

namespace HeatTreatFurnace::Test
{
    TEST_SUITE("ManualTempState")
    {
        TEST_CASE("MANUAL_TEMP: Transitions to OFF")
        {
            FsmTestFixture fixture;
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));
            fixture.Init();

            fixture.fsm.Post(EvtModeManual());
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::MANUAL);

            fixture.fsm.Post(EvtModeOff());
            fixture.fsm.ProcessQueue();
            // In ManualState.cpp, EvtModeOff returns STATE_PROFILE. This is likely a bug.
            // But I will first test what is there and see it fail if I expect STATE_OFF.
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::OFF);
        }

        TEST_CASE("MANUAL_TEMP: Transitions to PROFILE when no profile already loaded")
        {
            FsmTestFixture fixture;
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));
            fixture.Init();

            fixture.fsm.Post(EvtModeManual());
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::MANUAL);

            fixture.fsm.Post(EvtModeProfile());
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE);
        }

        TEST_CASE("MANUAL_TEMP: Transitions to PROFILE_LOAD when a profile was previously loaded")
        {
            FsmTestFixture fixture;
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));
            fixture.Init();

            Profile profile;
            fixture.fsm.LoadProfile(profile);

            fixture.fsm.Post(EvtModeManual());
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::MANUAL);

            fixture.fsm.Post(EvtModeProfile());
            fixture.fsm.ProcessQueue();
            // ProfileState::on_enter_state posts EvtProfileAlreadyLoaded if a profile is set.
            fixture.fsm.ProcessQueue(); 
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_LOADED);
        }

        TEST_CASE("MANUAL_TEMP: EvtManualSetTemp stays in MANUAL_TEMP")
        {
            FsmTestFixture fixture;
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));
            fixture.Init();

            fixture.fsm.Post(EvtModeManual());
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::MANUAL);

            fixture.fsm.Post(EvtManualSetTemp(100.0f));
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::MANUAL);
        }

        TEST_CASE("MANUAL_TEMP: EvtError transitions to ERROR")
        {
            FsmTestFixture fixture;
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));
            fixture.Init();

            fixture.fsm.Post(EvtModeManual());
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::MANUAL);

            fixture.fsm.Post(EvtError(Error::SafetyInterlock, Domain::Furnace, "Test error"));
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::ERROR);
        }
    }
} // namespace HeatTreatFurnace::Test
