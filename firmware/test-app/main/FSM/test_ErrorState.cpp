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
            REQUIRE(!fixture.mockHeater.IsEnabled());

            EvtError evtErr(Error::SensorFailure, Domain::Furnace, "Test error");
            fixture.fsm.Post(evtErr);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::ERROR);
            REQUIRE(!fixture.mockHeater.IsEnabled());

            EvtModeOff evct;
            fixture.fsm.Post(evct);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::OFF);
            REQUIRE(!fixture.mockHeater.IsEnabled());
        }

        // TEST_CASE("ERROR: Error transitions to MANUAL")
        // {
        //     FsmTestFixture fixture;
        //
        //     ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));
        //     REQUIRE_CALL(fixture.mockHeater, Disable()).RETURN(true).TIMES(1);
        //     fixture.Init();
        //     fixture.fsm.ProcessQueue();
        //     REQUIRE(fixture.fsm.GetCurrentState() == StateId::OFF);
        //
        //     // REQUIRE_CALL(fixture.mockHeater, Disable()).RETURN(true).TIMES(1);
        //     EvtError evtErr(Error::SensorFailure, Domain::Furnace, "Test error");
        //     fixture.fsm.Post(evtErr);
        //     fixture.fsm.ProcessQueue();
        //     REQUIRE(fixture.fsm.GetCurrentState() == StateId::ERROR);
        //
        //     REQUIRE_CALL(fixture.mockHeater, IsEnabled()).RETURN(false).TIMES(1);
        //     EvtModeManual evct;
        //     fixture.fsm.Post(evct);
        //     fixture.fsm.ProcessQueue();
        //     // REQUIRE(fixture.fsm.GetCurrentState() == StateId::MANUAL_OFF);
        // }

        TEST_CASE("ERROR: EvtModeProfile transitions to PROFILE")
        {
            FsmTestFixture fixture;

            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));

            fixture.Init();
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::OFF);

            EvtError evtErr(Error::SensorFailure, Domain::Furnace, "Test error");
            fixture.fsm.Post(evtErr);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::ERROR);

            EvtModeProfile evct;
            fixture.fsm.Post(evct);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE);
            REQUIRE(!fixture.mockHeater.IsEnabled());
        }

        TEST_CASE("ERROR: EvtProfile transitions to PROFILE_LOADED when profile was already loaded when an error occurred")
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

            EvtError evtErr(Error::SensorFailure, Domain::Furnace, "Test error");
            fixture.fsm.Post(evtErr);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::ERROR);
            REQUIRE(!fixture.mockHeater.IsEnabled());

            EvtModeProfile evtProf;
            fixture.fsm.Post(evtProf);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_LOADED);
            REQUIRE(!fixture.mockHeater.IsEnabled());
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

            EvtError evtErr(Error::SensorFailure, Domain::Furnace, "Test error");
            fixture.fsm.Post(evtErr);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::ERROR);
            REQUIRE(!fixture.mockHeater.IsEnabled());

            EvtModeProfile evtProf;
            fixture.fsm.Post(evtProf);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE);
            REQUIRE(!fixture.mockHeater.IsEnabled());
        }
    }
} // namespace HeatTreatFurnace::Test
