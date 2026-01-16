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
            EvtModeProfile evct;
            fixture.fsm.Post(evct);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE);
        }

        TEST_CASE("PROFILE_STATE: Re-enters PROFILE_LOADED on mode change")
        {
            FsmTestFixture fixture;
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));
            fixture.Init();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::OFF);
            Profile profile;
            fixture.fsm.LoadProfile(profile);

            EvtModeProfile evct;
            fixture.fsm.Post(evct);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_LOADED);
        }

        TEST_CASE("PROFILE_STATE: EvtError transitions to ERROR")
        {
            FsmTestFixture fixture;
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));
            fixture.Init();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::OFF);
            EvtModeProfile evct;
            fixture.fsm.Post(evct);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE);

            EvtError evt(Error::SensorFailure, Domain::Furnace, "Test error");
            fixture.fsm.Post(evt);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::ERROR);
        }
    }
} // namespace HeatTreatFurnace::Test
