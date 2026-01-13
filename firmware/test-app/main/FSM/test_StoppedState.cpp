#include <doctest/doctest.h>
#include <doctest/trompeloeil.hpp>
#include "FsmTestFixture.hpp"

namespace HeatTreatFurnace::Test
{
    TEST_SUITE("StoppedState")
    {
        TEST_CASE("STOPPED: EvtStart transitions to RUNNING when RUNNING profile is stopped and resumed")
        {
            FsmTestFixture fixture;
            Profile profile;

            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1);
            fixture.Init();

            // Transition to PAUSED
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

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_RUNNING);

            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Received EvtPause"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Pausing program execution"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Exiting RUNNING state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("StoppedState"),etl::string_view("Entered PAUSED state"))).TIMES(1);
            EvtProfileStop pauseEvt;
            fixture.fsm.Post(pauseEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_STOPPED);

            // Then resume
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("StoppedState"),etl::string_view("Resuming program execution"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("StoppedState"),etl::string_view("Exiting PAUSED state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Entered RUNNING state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("StoppedState"),etl::string_view("Received EvtResume"))).TIMES(1);
            EvtProfileStart evt;
            fixture.fsm.Post(evt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_RUNNING);
        }

        TEST_CASE("PAUSED: EvtProfileClear transitions to PROFILE")
        {
            FsmTestFixture fixture;
            Profile profile;

            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1);
            fixture.Init();

            // Transition to PAUSED
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

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_RUNNING);

            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Received EvtPause"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Pausing program execution"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Exiting RUNNING state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("StoppedState"),etl::string_view("Entered PAUSED state"))).TIMES(1);
            EvtProfileStop pauseEvt;
            fixture.fsm.Post(pauseEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_STOPPED);

            // Then clear
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("StoppedState"),etl::string_view("Cancelling program execution"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("StoppedState"),etl::string_view("Exiting PAUSED state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("CancelledState"),etl::string_view("Entered CANCELLED state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("StoppedState"),etl::string_view("Received EvtProfileStop"))).TIMES(1);
            EvtProfileClear evt;
            fixture.fsm.Post(evt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE);
        }

        TEST_CASE("PAUSED: EvtError transitions to ERROR")
        {
            FsmTestFixture fixture;
            Profile profile;

            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1);
            fixture.Init();

            // Transition to PAUSED
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
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Received EvtPause"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Pausing program execution"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Exiting RUNNING state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("StoppedState"),etl::string_view("Entered PAUSED state"))).TIMES(1);
            EvtProfileStop pauseEvt;
            fixture.fsm.Post(pauseEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_STOPPED);

            // Then error
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("StoppedState"),etl::string_view("Received EvtError: Test error"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("StoppedState"),etl::string_view("Exiting PAUSED state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ErrorState"),etl::string_view("Entered ERROR state"))).TIMES(1);
            EvtError evt(Error::SensorFailure, Domain::Furnace, "Test error");
            fixture.fsm.Post(evt, EventPriority::Critical);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::ERROR);
        }
    }
} // namespace HeatTreatFurnace::Test
