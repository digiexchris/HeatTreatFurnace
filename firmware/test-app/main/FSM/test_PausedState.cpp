#include <doctest/doctest.h>
#include <doctest/trompeloeil.hpp>
#include "FsmTestFixture.hpp"

namespace HeatTreatFurnace::Test
{
    TEST_SUITE("PausedState")
    {
        TEST_CASE("PAUSED: EvtResume transitions to RUNNING")
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
            EvtLoadProfile loadEvt(profile);
            fixture.fsm.Post(loadEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"),etl::string_view("Received EvtStart"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"),etl::string_view("Starting program execution"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"),etl::string_view("Exiting LOADED state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Entered RUNNING state"))).TIMES(1);
            EvtStart startEvt;
            fixture.fsm.Post(startEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Received EvtPause"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Pausing program execution"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Exiting RUNNING state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("PausedState"),etl::string_view("Entered PAUSED state"))).TIMES(1);
            EvtPause pauseEvt;
            fixture.fsm.Post(pauseEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            // Then resume
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("PausedState"),etl::string_view("Resuming program execution"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("PausedState"),etl::string_view("Exiting PAUSED state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Entered RUNNING state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("PausedState"),etl::string_view("Received EvtResume"))).TIMES(1);
            EvtResume evt;
            fixture.fsm.Post(evt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::RUNNING);
        }

        TEST_CASE("PAUSED: EvtCancel transitions to CANCELLED")
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
            EvtLoadProfile loadEvt(profile);
            fixture.fsm.Post(loadEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"),etl::string_view("Received EvtStart"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"),etl::string_view("Starting program execution"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"),etl::string_view("Exiting LOADED state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Entered RUNNING state"))).TIMES(1);
            EvtStart startEvt;
            fixture.fsm.Post(startEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Received EvtPause"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Pausing program execution"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Exiting RUNNING state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("PausedState"),etl::string_view("Entered PAUSED state"))).TIMES(1);
            EvtPause pauseEvt;
            fixture.fsm.Post(pauseEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            // Then cancel
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("PausedState"),etl::string_view("Cancelling program execution"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("PausedState"),etl::string_view("Exiting PAUSED state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("CancelledState"),etl::string_view("Entered CANCELLED state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("PausedState"),etl::string_view("Received EvtCancel"))).TIMES(1);
            EvtCancel evt;
            fixture.fsm.Post(evt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::CANCELLED);
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
            EvtLoadProfile loadEvt(profile);
            fixture.fsm.Post(loadEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"),etl::string_view("Received EvtStart"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"),etl::string_view("Starting program execution"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"),etl::string_view("Exiting LOADED state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Entered RUNNING state"))).TIMES(1);
            EvtStart startEvt;
            fixture.fsm.Post(startEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Received EvtPause"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Pausing program execution"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Exiting RUNNING state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("PausedState"),etl::string_view("Entered PAUSED state"))).TIMES(1);
            EvtPause pauseEvt;
            fixture.fsm.Post(pauseEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            // Then error
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("PausedState"),etl::string_view("Received EvtError: Test error"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("PausedState"),etl::string_view("Exiting PAUSED state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ErrorState"),etl::string_view("Entered ERROR state"))).TIMES(1);
            EvtError evt(Error::SensorFailure, Domain::Furnace, "Test error");
            fixture.fsm.Post(evt, EventPriority::Critical);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::ERROR);
        }
    }
} // namespace HeatTreatFurnace::Test
