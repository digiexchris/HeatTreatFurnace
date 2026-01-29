#include <doctest/doctest.h>
#include <doctest/trompeloeil.hpp>
#include "FsmTestFixture.hpp"

namespace HeatTreatFurnace::Test
{
    using namespace trompeloeil;

    TEST_SUITE("ManualTempState")
    {
        TEST_CASE("MANUAL_TEMP: Transitions to OFF")
        {
            FsmTestFixture fixture;
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));

            // REQUIRE_CALL(fixture.mockHeater, Disable()).RETURN(true).TIMES(1); //For Init(), when it transitions into the initial OFF state
            fixture.Init();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::OFF);
            REQUIRE(!fixture.mockHeater.IsEnabled());

            fixture.fsm.Post(EvtModeManual());
            fixture.fsm.ProcessQueue(); //Will process two events, the EvtModeManual, and the auto transition via EvtManualOff
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::MANUAL_OFF);

            fixture.fsm.Post(EvtModeOff());
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::OFF);
            REQUIRE(!fixture.mockHeater.IsEnabled());
        }

        TEST_CASE("MANUAL_TEMP: Transitions to PROFILE from MANUAL_OFF when no profile already loaded")
        {
            FsmTestFixture fixture;
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));

            fixture.Init();
            REQUIRE(!fixture.mockHeater.IsEnabled());
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::OFF);

            fixture.fsm.Post(EvtModeManual());
            fixture.fsm.ProcessQueue(); //Will process two events, the EvtModeManual, and the auto transition via EvtManualOff
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::MANUAL_OFF);
            REQUIRE(!fixture.mockHeater.IsEnabled());

            fixture.fsm.Post(EvtModeProfile());
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE);
            REQUIRE(!fixture.mockHeater.IsEnabled());
        }

        TEST_CASE("MANUAL_TEMP: Transitions to PROFILE from MANUAL_ON when no profile already loaded")
        {
            FsmTestFixture fixture;
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));

            fixture.Init();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::OFF);

            fixture.fsm.Post(EvtModeManual());
            fixture.fsm.ProcessQueue(); //Will process two events, the EvtModeManual, and the auto transition via EvtManualOff
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::MANUAL_OFF);
            REQUIRE(!fixture.mockHeater.IsEnabled());

            fixture.fsm.Post(EvtManualSetOn());
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::MANUAL_ON);
            REQUIRE(fixture.mockHeater.IsEnabled()); //TURNED ON

            fixture.fsm.Post(EvtModeProfile());
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE);
            REQUIRE(!fixture.mockHeater.IsEnabled()); //TURNED BACK OFF
        }

        TEST_CASE("MANUAL_TEMP: Transitions to PROFILE_LOAD from MANUAL_OFF when a profile was previously loaded")
        {
            FsmTestFixture fixture;
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));

            fixture.Init();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::OFF);

            Profile profile;
            profile.isValid = true;
            fixture.fsm.LoadProfile(profile);

            fixture.fsm.Post(EvtModeManual());
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::MANUAL_OFF);
            REQUIRE(!fixture.mockHeater.IsEnabled());

            fixture.fsm.Post(EvtModeProfile());
            fixture.fsm.ProcessQueue();
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_LOADED);
            REQUIRE(!fixture.mockHeater.IsEnabled());
        }

        TEST_CASE("MANUAL_TEMP: Transitions to PROFILE_LOAD from MANUAL_ON when a profile is already loaded")
        {
            FsmTestFixture fixture;
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));

            fixture.Init();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::OFF);
            REQUIRE(!fixture.mockHeater.IsEnabled());

            Profile profile;
            profile.isValid = true;
            fixture.fsm.LoadProfile(profile);

            fixture.fsm.Post(EvtModeManual());
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::MANUAL_OFF);
            REQUIRE(!fixture.mockHeater.IsEnabled());

            fixture.fsm.Post(EvtManualSetOn());
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::MANUAL_ON);
            REQUIRE(fixture.mockHeater.IsEnabled()); //TURNED ON
            fixture.fsm.Post(EvtModeProfile());
            fixture.fsm.ProcessQueue();
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_LOADED);
            REQUIRE(!fixture.mockHeater.IsEnabled()); //BACK OFF
        }

        TEST_CASE("MANUAL_TEMP: EvtManualSetTemp stays in MANUAL_TEMP")
        {
            FsmTestFixture fixture;
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));

            fixture.Init();

            fixture.fsm.Post(EvtModeManual());
            fixture.fsm.ProcessQueue();
            REQUIRE(!fixture.mockHeater.IsEnabled());
            //MANUAL auto-transitions to MANUAL_OFF
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::MANUAL_OFF);
            REQUIRE(!fixture.mockHeater.IsEnabled());

            REQUIRE_CALL(fixture.mockHeater, SetTargetTemp(100.0f)).RETURN(true);
            fixture.fsm.Post(EvtManualSetTemp(100.0f));
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::MANUAL_OFF);
            REQUIRE(!fixture.mockHeater.IsEnabled());
        }

        TEST_CASE("MANUAL_TEMP: EvtError transitions to ERROR from ManualOff")
        {
            FsmTestFixture fixture;
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));

            fixture.Init();

            fixture.fsm.Post(EvtModeManual());
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::MANUAL_OFF);

            fixture.fsm.Post(EvtError(Error::SafetyInterlock, Domain::Furnace, "Test error"));
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::ERROR);
        }

        TEST_CASE("MANUAL_TEMP: EvtError transitions to ERROR from ManualOn")
        {
            FsmTestFixture fixture;
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));

            fixture.Init();

            fixture.fsm.Post(EvtModeManual());
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::MANUAL_OFF);
            REQUIRE(!fixture.mockHeater.IsEnabled());

            fixture.fsm.Post(EvtManualSetOn());
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::MANUAL_ON);
            REQUIRE(fixture.mockHeater.IsEnabled());

            fixture.fsm.Post(EvtError(Error::SafetyInterlock, Domain::Furnace, "Test error"));
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::ERROR);
            REQUIRE(!fixture.mockHeater.IsEnabled());
        }
    }
} // namespace HeatTreatFurnace::Test
