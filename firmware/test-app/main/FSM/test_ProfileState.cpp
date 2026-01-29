#include <doctest/doctest.h>
#include <doctest/trompeloeil.hpp>

#include "FsmTestFixture.hpp"

namespace HeatTreatFurnace::Test
{
    TEST_SUITE("ProfileState")
    {
        TEST_CASE("PROFILE_STATE: Initial")
        {
            FsmTestFixture fixture;
            Profile profile;

            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));
            fixture.Init();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::OFF);
            REQUIRE(!fixture.mockHeater.IsEnabled());
            EvtModeProfile evct;
            fixture.fsm.Post(evct);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE);
            REQUIRE(!fixture.mockHeater.IsEnabled());
        }

        TEST_CASE("PROFILE_STATE: PROFILE transitions to PROFILE_LOADED with EvtProfileLoad event")
        {
            REQUIRE(false);
        }

        TEST_CASE("PROFILE_STATE: PROFILE transitions to PROFILE_LOADED with EvtModeProfile event from OFF")
        {
            REQUIRE(false);
        }

        TEST_CASE("PROFILE_STATE: Re-enters PROFILE_LOADED on mode change")
        {
            FsmTestFixture fixture;

            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));
            fixture.Init();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::OFF);
            Profile profile;
            fixture.fsm.LoadProfile(profile);
            REQUIRE(!fixture.mockHeater.IsEnabled());

            EvtModeProfile evct;
            fixture.fsm.Post(evct);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_LOADED);
            REQUIRE(!fixture.mockHeater.IsEnabled());
        }

        TEST_CASE("PROFILE_STATE: EvtError transitions to ERROR")
        {
            FsmTestFixture fixture;
            fixture.Init();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::OFF);
            EvtModeProfile evct;
            fixture.fsm.Post(evct);
            fixture.fsm.ProcessQueue();
            REQUIRE(!fixture.mockHeater.IsEnabled());

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE);

            EvtError evt(Error::SensorFailure, Domain::Furnace, "Test error");
            fixture.fsm.Post(evt);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::ERROR);
            REQUIRE(!fixture.mockHeater.IsEnabled());
        }
    }
} // namespace HeatTreatFurnace::Test
