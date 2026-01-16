#include <doctest/doctest.h>
#include <doctest/trompeloeil.hpp>
#include "FsmTestFixture.hpp"

namespace HeatTreatFurnace::Test
{
    TEST_SUITE("StoppedState")
    {
        TEST_CASE("STOPPED: EvtProfileLoad transitions to LOADED")
        {
            FsmTestFixture fixture;
            Profile profile;
            profile.name = "Test Profile";
            Profile profile2;
            profile2.name = "Test Profile 2";

            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1);
            fixture.Init();

            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("OffState"),etl::string_view("Exiting OFF state"))).TIMES(1);
            EvtModeProfile profileEvt;
            fixture.fsm.Post(profileEvt);
            fixture.fsm.ProcessQueue();

            //Transition to STOPPED
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ProfileLoadedState"),etl::string_view("Entered LOADED state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ProfileLoadedState"),etl::string_view("Exiting LOADED state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_, etl::string_view("ProfileLoadedState"),etl::string_view("Received EvtProfileStart"))).TIMES(1);

            EvtProfileLoad loadEvt1(profile);
            fixture.fsm.Post(loadEvt1);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_LOADED);

            EvtProfileStart startEvt;
            fixture.fsm.Post(startEvt);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_RUNNING);

            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ProfileStoppedState"),etl::string_view("Entered STOPPED state"))).TIMES(1);
            EvtProfileStop cancelEvt;
            fixture.fsm.Post(cancelEvt);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_STOPPED);

            // // Then load new profile
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ProfileStoppedState"),etl::string_view("Received EvtProfileLoad"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ProfileStoppedState"),etl::string_view("Profile loaded, transitioning to PROFILE"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ProfileStoppedState"),etl::string_view("Exiting STOPPED state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_, etl::string_view("ProfileLoadedState"),etl::string_view("Entered LOADED state"))).TIMES(1);
            EvtProfileLoad loadEvt2(profile2);
            fixture.fsm.Post(loadEvt2);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_LOADED);

            REQUIRE_EQ(fixture.fsm.GetCurrentProfile().name, profile2.name);
        }

        TEST_CASE("STOPPED: EvtProfileClear transitions to IDLE")
        {
            FsmTestFixture fixture;
            Profile profile;
            profile.name = "Test Profile";
            Profile profile2;
            profile2.name = "Test Profile 2";

            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1);
            fixture.Init();

            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("OffState"),etl::string_view("Exiting OFF state"))).TIMES(1);
            EvtModeProfile profileEvt;
            fixture.fsm.Post(profileEvt);
            fixture.fsm.ProcessQueue();

            //Transition to STOPPED
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ProfileLoadedState"),etl::string_view("Entered LOADED state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ProfileLoadedState"),etl::string_view("Exiting LOADED state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_, etl::string_view("ProfileLoadedState"),etl::string_view("Received EvtProfileStart"))).TIMES(1);

            EvtProfileLoad loadEvt1(profile);
            fixture.fsm.Post(loadEvt1);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_LOADED);

            EvtProfileStart startEvt;
            fixture.fsm.Post(startEvt);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_RUNNING);

            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ProfileStoppedState"),etl::string_view("Entered STOPPED state"))).TIMES(1);
            EvtProfileStop cancelEvt;
            fixture.fsm.Post(cancelEvt);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_STOPPED);

            // // Then clear profile
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ProfileStoppedState"),etl::string_view("Received EvtProfileClear"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ProfileStoppedState"),etl::string_view("Program cleared, returning to Profile"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ProfileStoppedState"),etl::string_view("Exiting STOPPED state"))).TIMES(1);
            EvtProfileClear clearEvt;
            fixture.fsm.Post(clearEvt);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE);

            REQUIRE(!fixture.fsm.IsProfileSet());
        }

        TEST_CASE("STOPPED: EvtError transitions to ERROR")
        {
            FsmTestFixture fixture;
            Profile profile;
            profile.name = "Test Profile";
            Profile profile2;
            profile2.name = "Test Profile 2";

            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1);
            fixture.Init();

            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("OffState"),etl::string_view("Exiting OFF state"))).TIMES(1);
            EvtModeProfile profileEvt;
            fixture.fsm.Post(profileEvt);
            fixture.fsm.ProcessQueue();

            //Transition to STOPPED
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ProfileLoadedState"),etl::string_view("Entered LOADED state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ProfileLoadedState"),etl::string_view("Exiting LOADED state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_, etl::string_view("ProfileLoadedState"),etl::string_view("Received EvtProfileStart"))).TIMES(1);

            EvtProfileLoad loadEvt1(profile);
            fixture.fsm.Post(loadEvt1);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_LOADED);

            EvtProfileStart startEvt;
            fixture.fsm.Post(startEvt);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_RUNNING);

            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ProfileStoppedState"),etl::string_view("Entered STOPPED state"))).TIMES(1);
            EvtProfileStop cancelEvt;
            fixture.fsm.Post(cancelEvt);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_STOPPED);

            // // Then send error
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ProfileStoppedState"),etl::string_view("Received EvtError: Test error"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ProfileStoppedState"),etl::string_view("Exiting STOPPED state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ErrorState"),etl::string_view("Entered ERROR state"))).TIMES(1);
            EvtError errorevt(Error::SensorFailure, Domain::Furnace, "Test error");
            fixture.fsm.Post(errorevt);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::ERROR);
        }
    }
} // namespace HeatTreatFurnace::Test
