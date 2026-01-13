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

            // Transition to COMPLETED
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("IdleState"),etl::string_view("Profile loaded, transitioning to LOADED"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("IdleState"),etl::string_view("Exiting IDLE state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"),etl::string_view("Entered LOADED state"))).TIMES(1);
            EvtProfileLoad loadEvt(profile);
            fixture.fsm.Post(loadEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"),etl::string_view("Received EvtProfileStart"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"),etl::string_view("Starting program execution"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"),etl::string_view("Exiting LOADED state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Entered RUNNING state"))).TIMES(1);
            EvtProfileStart startEvt;
            fixture.fsm.Post(startEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Received EvtProgramComplete"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Profile execution completed"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Exiting RUNNING state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("CompletedState"),etl::string_view("Entered COMPLETED state"))).TIMES(1);
            EvtProfileComplete completeEvt;
            fixture.fsm.Post(completeEvt, EventPriority::Furnace);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_COMPLETED);

            // Then clear program
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("CompletedState"),etl::string_view("Received EvtProfileClear"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("CompletedState"),etl::string_view("Program cleared, returning to IDLE"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("CompletedState"),etl::string_view("Exiting COMPLETED state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("IdleState"),etl::string_view("Entered IDLE state"))).TIMES(1);
            EvtProfileClear evt;
            fixture.fsm.Post(evt, EventPriority::UI);
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

            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1);
            fixture.Init();

            // Transition to COMPLETED
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("IdleState"),etl::string_view("Profile loaded, transitioning to LOADED"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("IdleState"),etl::string_view("Exiting IDLE state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"),etl::string_view("Entered LOADED state"))).TIMES(1);
            EvtProfileLoad loadEvt1(profile);
            fixture.fsm.Post(loadEvt1, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_LOADED);

            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"),etl::string_view("Received EvtProfileStart"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"),etl::string_view("Starting program execution"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"),etl::string_view("Exiting LOADED state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Entered RUNNING state"))).TIMES(1);
            EvtProfileStart startEvt;
            fixture.fsm.Post(startEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_RUNNING);

            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Received EvtProgramComplete"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Profile execution completed"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Exiting RUNNING state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("CompletedState"),etl::string_view("Entered COMPLETED state"))).TIMES(1);
            EvtProfileComplete completeEvt;
            fixture.fsm.Post(completeEvt, EventPriority::Furnace);
            fixture.fsm.ProcessQueue();

            std::shared_ptr<Profile> loadedProfile = fixture.fsm.GetCurrentProfile();

            //the data didn't just dissapear when we loaded the profile
            REQUIRE(loadedProfile->runCompleted == false);
            REQUIRE(loadedProfile->currentSegment == 1);
            REQUIRE((loadedProfile->currentSegmentTime == std::chrono::seconds{450}));

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_COMPLETED);
            std::shared_ptr<Profile> currentProfile = fixture.fsm.GetCurrentProfile();
            REQUIRE(currentProfile->runCompleted == true);

            // Then start profile
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("CompletedState"),etl::string_view("Received EvtProfileLoad"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("CompletedState"),etl::string_view("Profile loaded, transitioning to LOADED"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("CompletedState"),etl::string_view("Exiting COMPLETED state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"),etl::string_view("Entered LOADED state"))).TIMES(1);
            EvtProfileStart startEvt2;
            fixture.fsm.Post(startEvt2, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_RUNNING);
            REQUIRE(currentProfile->runCompleted == false);
            REQUIRE(currentProfile->currentSegment == 0);
            REQUIRE((currentProfile->currentSegmentTime == std::chrono::seconds(0)));
        }

        TEST_CASE("COMPLETED: EvtProfileLoad transitions to LOADED")
        {
            FsmTestFixture fixture;
            Profile profile;

            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1);
            fixture.Init();

            // Transition to COMPLETED
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("IdleState"),etl::string_view("Profile loaded, transitioning to LOADED"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("IdleState"),etl::string_view("Exiting IDLE state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"),etl::string_view("Entered LOADED state"))).TIMES(1);
            EvtProfileLoad loadEvt1(profile);
            fixture.fsm.Post(loadEvt1, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"),etl::string_view("Received EvtProfileStart"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"),etl::string_view("Starting program execution"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"),etl::string_view("Exiting LOADED state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Entered RUNNING state"))).TIMES(1);
            EvtProfileStart startEvt;
            fixture.fsm.Post(startEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Received EvtProgramComplete"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Profile execution completed"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Exiting RUNNING state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("CompletedState"),etl::string_view("Entered COMPLETED state"))).TIMES(1);
            EvtProfileComplete completeEvt;
            fixture.fsm.Post(completeEvt, EventPriority::Furnace);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_COMPLETED);

            // Then load new profile
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("CompletedState"),etl::string_view("Received EvtProfileLoad"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("CompletedState"),etl::string_view("Profile loaded, transitioning to LOADED"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("CompletedState"),etl::string_view("Exiting COMPLETED state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"),etl::string_view("Entered LOADED state"))).TIMES(1);
            EvtProfileLoad loadEvt2(profile);
            fixture.fsm.Post(loadEvt2, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_LOADED);
        }

        TEST_CASE("COMPLETED: EvtManualSetTemp transitions to MANUAL_TEMP")
        {
            FsmTestFixture fixture;
            Profile profile;

            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1);
            fixture.Init();

            // Transition to COMPLETED
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("IdleState"),etl::string_view("Profile loaded, transitioning to LOADED"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("IdleState"),etl::string_view("Exiting IDLE state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"),etl::string_view("Entered LOADED state"))).TIMES(1);
            EvtProfileLoad loadEvt(profile);
            fixture.fsm.Post(loadEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"),etl::string_view("Received EvtProfileStart"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"),etl::string_view("Starting program execution"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"),etl::string_view("Exiting LOADED state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Entered RUNNING state"))).TIMES(1);
            EvtProfileStart startEvt;
            fixture.fsm.Post(startEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Received EvtProgramComplete"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Profile execution completed"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Exiting RUNNING state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("CompletedState"),etl::string_view("Entered COMPLETED state"))).TIMES(1);
            EvtProfileComplete completeEvt;
            fixture.fsm.Post(completeEvt, EventPriority::Furnace);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_COMPLETED);

            // Then set manual temp
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("CompletedState"),_)).TIMES(1); // "Received EvtManualSetTemp, target: 100 C"
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("CompletedState"),etl::string_view("Transitioning to MANUAL_TEMP mode"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("CompletedState"),etl::string_view("Exiting COMPLETED state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ManualTempState"),etl::string_view("Entered MANUAL_TEMP state"))).TIMES(1);
            EvtManualSetTemp evt(100.0f);
            fixture.fsm.Post(evt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::MANUAL);
        }

        TEST_CASE("COMPLETED: EvtError transitions to ERROR")
        {
            FsmTestFixture fixture;
            Profile profile;

            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1);
            fixture.Init();

            // Transition to COMPLETED
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("IdleState"),etl::string_view("Profile loaded, transitioning to LOADED"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("IdleState"),etl::string_view("Exiting IDLE state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"),etl::string_view("Entered LOADED state"))).TIMES(1);
            EvtProfileLoad loadEvt(profile);
            fixture.fsm.Post(loadEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"),etl::string_view("Received EvtProfileStart"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"),etl::string_view("Starting program execution"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"),etl::string_view("Exiting LOADED state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Entered RUNNING state"))).TIMES(1);
            EvtProfileStart startEvt;
            fixture.fsm.Post(startEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Received EvtProgramComplete"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Profile execution completed"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Exiting RUNNING state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("CompletedState"),etl::string_view("Entered COMPLETED state"))).TIMES(1);
            EvtProfileComplete completeEvt;
            fixture.fsm.Post(completeEvt, EventPriority::Furnace);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_COMPLETED);

            // Then error
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("CompletedState"),_)).TIMES(1); // "Received EvtError: Test error"
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("CompletedState"),etl::string_view("Exiting COMPLETED state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ErrorState"),etl::string_view("Entered ERROR state"))).TIMES(1);
            EvtError evt(Error::SensorFailure, Domain::Furnace, "Test error");
            fixture.fsm.Post(evt, EventPriority::Critical);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::ERROR);
        }
    }
} // namespace HeatTreatFurnace::Test
