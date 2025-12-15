#include <doctest/doctest.h>
#include <doctest/trompeloeil.hpp>
#include "FsmTestFixture.hpp"

namespace HeatTreatFurnace::Test
{
    TEST_SUITE("CompletedState")
    {
        TEST_CASE("COMPLETED: EvtClearProgram transitions to IDLE")
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
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Received EvtComplete"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Profile execution completed"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Exiting RUNNING state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("CompletedState"),etl::string_view("Entered COMPLETED state"))).TIMES(1);
            EvtComplete completeEvt;
            fixture.fsm.Post(completeEvt, EventPriority::Furnace);
            fixture.fsm.ProcessQueue();

            // Then clear program
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("CompletedState"),etl::string_view("Received EvtClearProgram"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("CompletedState"),etl::string_view("Program cleared, returning to IDLE"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("CompletedState"),etl::string_view("Exiting COMPLETED state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("IdleState"),etl::string_view("Entered IDLE state"))).TIMES(1);
            EvtClearProgram evt;
            fixture.fsm.Post(evt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::IDLE);
        }

        TEST_CASE("COMPLETED: EvtLoadProfile transitions to LOADED")
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
            EvtLoadProfile loadEvt1(profile);
            fixture.fsm.Post(loadEvt1, EventPriority::UI);
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
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Received EvtComplete"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Profile execution completed"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Exiting RUNNING state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("CompletedState"),etl::string_view("Entered COMPLETED state"))).TIMES(1);
            EvtComplete completeEvt;
            fixture.fsm.Post(completeEvt, EventPriority::Furnace);
            fixture.fsm.ProcessQueue();

            // Then load new profile
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("CompletedState"),etl::string_view("Received EvtLoadProfile"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("CompletedState"),etl::string_view("Profile loaded, transitioning to LOADED"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("CompletedState"),etl::string_view("Exiting COMPLETED state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"),etl::string_view("Entered LOADED state"))).TIMES(1);
            EvtLoadProfile loadEvt2(profile);
            fixture.fsm.Post(loadEvt2, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::LOADED);
        }

        TEST_CASE("COMPLETED: EvtSetManualTemp transitions to MANUAL_TEMP")
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
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Received EvtComplete"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Profile execution completed"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Exiting RUNNING state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("CompletedState"),etl::string_view("Entered COMPLETED state"))).TIMES(1);
            EvtComplete completeEvt;
            fixture.fsm.Post(completeEvt, EventPriority::Furnace);
            fixture.fsm.ProcessQueue();

            // Then set manual temp
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("CompletedState"),_)).TIMES(1); // "Received EvtSetManualTemp, target: 100 C"
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("CompletedState"),etl::string_view("Transitioning to MANUAL_TEMP mode"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("CompletedState"),etl::string_view("Exiting COMPLETED state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ManualTempState"),etl::string_view("Entered MANUAL_TEMP state"))).TIMES(1);
            EvtSetManualTemp evt(100.0f);
            fixture.fsm.Post(evt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::MANUAL_TEMP);
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
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Received EvtComplete"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Profile execution completed"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Exiting RUNNING state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("CompletedState"),etl::string_view("Entered COMPLETED state"))).TIMES(1);
            EvtComplete completeEvt;
            fixture.fsm.Post(completeEvt, EventPriority::Furnace);
            fixture.fsm.ProcessQueue();

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
