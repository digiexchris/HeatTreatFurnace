#include <doctest/doctest.h>
#include <doctest/trompeloeil.hpp>
#include "FsmTestFixture.hpp"

namespace HeatTreatFurnace::Test
{
    TEST_SUITE("CompletedState")
    {
        TEST_CASE("COMPLETED: EvtProfileClear transitions to IDLE")
        {
            FsmTestFixture fixture;
            Profile profile;

            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1);
            fixture.Init();

            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("OffState"),etl::string_view("Exiting OFF state"))).TIMES(1);
            EvtModeProfile profileEvt;
            fixture.fsm.Post(profileEvt);
            fixture.fsm.ProcessQueue();


            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ProfileLoadedState"),etl::string_view("Entered LOADED state"))).TIMES(1);
            EvtProfileLoad loadEvt(profile);
            fixture.fsm.Post(loadEvt);
            fixture.fsm.ProcessQueue();

            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ProfileLoadedState"),etl::string_view("Received EvtProfileStart"))).TIMES(1);
            // REQUIRE_CALL(fixture.mockLogBackend,
            //              WriteLog(_, etl::string_view("ProfileLoadedState"),etl::string_view("Starting program execution"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ProfileLoadedState"),etl::string_view("Exiting LOADED state"))).TIMES(1);
            // REQUIRE_CALL(fixture.mockLogBackend,
            //              WriteLog(_, etl::string_view("ProfileRunningState"),etl::string_view("Entered RUNNING state"))).TIMES(1);
            EvtProfileStart startEvt;
            fixture.fsm.Post(startEvt);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_RUNNING);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ProfileCompletedState"),etl::string_view("Entered COMPLETED state"))).TIMES(1);
            EvtProfileComplete completeEvt;
            fixture.fsm.Post(completeEvt);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_COMPLETED);

            // Then clear program
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ProfileCompletedState"),etl::string_view("Received EvtProfileClear"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ProfileCompletedState"),etl::string_view("Program cleared, returning to PROFILE"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ProfileCompletedState"),etl::string_view("Exiting COMPLETED state"))).TIMES(1);
            EvtProfileClear evt;
            fixture.fsm.Post(evt);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE);
        }

        TEST_CASE("COMPLETED: EvtProfileStart transitions to RUNNING, restarting from beginning")
        {
            FsmTestFixture fixture;
            Profile profile;
            ProfileSegment segment1;
            segment1.rampTime = std::chrono::seconds(200);
            segment1.dwellTime = std::chrono::seconds(300);

            ProfileSegment segment2;
            segment1.rampTime = std::chrono::seconds(400);
            segment1.dwellTime = std::chrono::seconds(500);

            profile.segments.push_back(segment1);
            profile.segments.push_back(segment2);

            //this isn't the right way to set a loaded profile in the future, but it's possible. We're just skipping ahead for the test.
            //it might make sense to allow this as the normal way to skip ahead in a profile.
            profile.currentSegment = 1;
            profile.currentSegmentTime = std::chrono::seconds(450);
            profile.runCompleted = false;

            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));
            fixture.Init();

            EvtModeProfile profileEvt;
            fixture.fsm.Post(profileEvt);
            fixture.fsm.ProcessQueue();

            // Transition to COMPLETED
            EvtProfileLoad loadEvt1(profile);
            fixture.fsm.Post(loadEvt1);
            fixture.fsm.ProcessQueue();


            Profile loadedProfile = fixture.fsm.GetCurrentProfile();

            //the data didn't just dissapear when we loaded the profile
            REQUIRE(loadedProfile.runCompleted == false);
            REQUIRE(loadedProfile.currentSegment == 1);
            REQUIRE((loadedProfile.currentSegmentTime == std::chrono::seconds{450}));
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_LOADED);

            EvtProfileStart startEvt;
            fixture.fsm.Post(startEvt);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_RUNNING);

            EvtProfileComplete completeEvt;
            fixture.fsm.Post(completeEvt);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_COMPLETED);
            Profile currentProfile = fixture.fsm.GetCurrentProfile();
            REQUIRE(currentProfile.runCompleted == true);

            // Then start profile
            EvtProfileStart startEvt2;
            fixture.fsm.Post(startEvt2);
            fixture.fsm.ProcessQueue();

            currentProfile = fixture.fsm.GetCurrentProfile();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_RUNNING);
            REQUIRE(currentProfile.runCompleted == false);
            REQUIRE(currentProfile.currentSegment == 0);
            REQUIRE((currentProfile.currentSegmentTime == std::chrono::seconds(0)));

            //Then complete the run
            // EvtProfileComplete completeEvt;
            // fixture.fsm.Post(completeEvt);
            // fixture.fsm.ProcessQueue();
            // currentProfile = fixture.fsm.GetCurrentProfile();
            // REQUIRE(currentProfile.runCompleted == true);
            // REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_COMPLETED);


        }

        TEST_CASE("COMPLETED: EvtProfileLoad transitions to LOADED")
        {
            FsmTestFixture fixture;
            Profile profile;

            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));
            fixture.Init();

            EvtModeProfile profileEvt;
            fixture.fsm.Post(profileEvt);
            fixture.fsm.ProcessQueue();

            // Transition to COMPLETED
            EvtProfileLoad loadEvt1(profile);
            fixture.fsm.Post(loadEvt1);
            fixture.fsm.ProcessQueue();

            EvtProfileStart startEvt;
            fixture.fsm.Post(startEvt);
            fixture.fsm.ProcessQueue();

            EvtProfileComplete completeEvt;
            fixture.fsm.Post(completeEvt);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_COMPLETED);

            // Then load new profile
            EvtProfileLoad loadEvt2(profile);
            fixture.fsm.Post(loadEvt2);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_LOADED);
        }

        TEST_CASE("COMPLETED: EvtError transitions to ERROR")
        {
            FsmTestFixture fixture;
            Profile profile;

            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));
            fixture.Init();

            EvtModeProfile profileEvt;
            fixture.fsm.Post(profileEvt);
            fixture.fsm.ProcessQueue();

            // Transition to COMPLETED
            EvtProfileLoad loadEvt(profile);
            fixture.fsm.Post(loadEvt);
            fixture.fsm.ProcessQueue();

            EvtProfileStart startEvt;
            fixture.fsm.Post(startEvt);
            fixture.fsm.ProcessQueue();

            EvtProfileComplete completeEvt;
            fixture.fsm.Post(completeEvt);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_COMPLETED);

            // Then error
            EvtError evt(Error::SensorFailure, Domain::Furnace, "Test error");
            fixture.fsm.Post(evt);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::ERROR);
        }
    }
} // namespace HeatTreatFurnace::Test
