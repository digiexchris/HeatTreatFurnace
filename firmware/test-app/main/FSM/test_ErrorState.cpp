#include <doctest/doctest.h>
#include <doctest/trompeloeil.hpp>
#include "FsmTestFixture.hpp"

namespace HeatTreatFurnace::Test
{
    TEST_SUITE("ErrorState")
    {
        TEST_CASE("ERROR: EvtProfileClear transitions to OFF")
        {
            FsmTestFixture fixture;

            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));
            fixture.Init();
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::OFF);

            // Transition to ERROR
            EvtError evtErr(Error::SensorFailure, Domain::Furnace, "Test error");
            fixture.fsm.Post(evtErr);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::ERROR);

            // Then reset
            EvtModeOff evct;
            fixture.fsm.Post(evct);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::OFF);


        }

        TEST_CASE("ERROR: EvtProfileLoad transitions to MANUAL")
        {
            FsmTestFixture fixture;

            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));
            fixture.Init();
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::OFF);

            // Transition to ERROR
            EvtError evtErr(Error::SensorFailure, Domain::Furnace, "Test error");
            fixture.fsm.Post(evtErr);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::ERROR);

            // Then reset
            EvtModeManual evct;
            fixture.fsm.Post(evct);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::MANUAL);
        }

        TEST_CASE("ERROR: EvtProfileLoad transitions to PROFILE")
        {
            FsmTestFixture fixture;

            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));
            fixture.Init();
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::OFF);

            // Transition to ERROR
            EvtError evtErr(Error::SensorFailure, Domain::Furnace, "Test error");
            fixture.fsm.Post(evtErr);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::ERROR);

            // Then reset
            EvtModeProfile evct;
            fixture.fsm.Post(evct);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE);
        }

        TEST_CASE("ERROR: EvtProfileLoad transitions to PROFILE_LOADED when profile was already loaded when an error occurred")
        {
            FsmTestFixture fixture;

            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));
            fixture.Init();
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::OFF);
            EvtModeProfile evct;
            fixture.fsm.Post(evct);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE);

            Profile profile;
            EvtProfileLoad evt(profile);
            fixture.fsm.Post(evt);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_LOADED);

            // Transition to ERROR
            EvtError evtErr(Error::SensorFailure, Domain::Furnace, "Test error");
            fixture.fsm.Post(evtErr);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::ERROR);

            // Then reset
            EvtModeProfile evtProf;
            fixture.fsm.Post(evtProf);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_LOADED);
        }

        TEST_CASE("ERROR: EvtProfileLoad transitions to PROFILE when profile was not already loaded when an error occurred")
        {
            FsmTestFixture fixture;

            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));
            fixture.Init();
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::OFF);
            EvtModeProfile evct;
            fixture.fsm.Post(evct);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE);

            // Transition to ERROR
            EvtError evtErr(Error::SensorFailure, Domain::Furnace, "Test error");
            fixture.fsm.Post(evtErr);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::ERROR);

            // Then reset
            EvtModeProfile evtProf;
            fixture.fsm.Post(evtProf);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE);
        }
    }
} // namespace HeatTreatFurnace::Test
