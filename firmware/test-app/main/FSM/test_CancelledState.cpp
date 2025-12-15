#include <doctest/doctest.h>
#include <doctest/trompeloeil.hpp>
#include "FsmTestFixture.hpp"

namespace HeatTreatFurnace::Test
{
    TEST_SUITE("CancelledState")
    {
        TEST_CASE("CANCELLED: EvtLoadProfile transitions to LOADED")
        {
            FsmTestFixture fixture;
            Profile profile;
            Profile profile2;

            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1);
            fixture.Init();

            // Transition to CANCELLED
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("IdleState"),etl::string_view("Profile loaded, transitioning to LOADED"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("IdleState"),etl::string_view("Exiting IDLE state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"),etl::string_view("Entered LOADED state"))).TIMES(1);
            EvtLoadProfile loadEvt1(profile);
            fixture.fsm.Post(loadEvt1, EventPriority::UI);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::LOADED);

            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"),etl::string_view("Starting program execution"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"),etl::string_view("Exiting LOADED state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Entered RUNNING state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_, etl::string_view("LoadedState"),etl::string_view("Received EvtStart"))).TIMES(1);
            EvtStart startEvt;
            fixture.fsm.Post(startEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Cancelling program execution"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"),etl::string_view("Exiting RUNNING state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("CancelledState"),etl::string_view("Entered CANCELLED state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_, etl::string_view("RunningState"),etl::string_view("Received EvtCancel"))).TIMES(1);
            EvtCancel cancelEvt;
            fixture.fsm.Post(cancelEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            // Then load new profile
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("CancelledState"),etl::string_view("Profile loaded, transitioning to LOADED"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("CancelledState"),etl::string_view("Exiting CANCELLED state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"),etl::string_view("Entered LOADED state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_, etl::string_view("CancelledState"),etl::string_view("Received EvtLoadProfile"))).TIMES(1);
            EvtLoadProfile loadEvt2(profile2);
            fixture.fsm.Post(loadEvt2, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::LOADED);
        }

        TEST_CASE("CANCELLED: EvtClearProgram transitions to IDLE")
        {
            FsmTestFixture fixture;
            Profile profile;

            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));
            fixture.Init();

            // Transition to CANCELLED
            EvtLoadProfile loadEvt(profile);
            fixture.fsm.Post(loadEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::LOADED);

            EvtStart startEvt;
            fixture.fsm.Post(startEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::RUNNING);

            EvtCancel cancelEvt;
            fixture.fsm.Post(cancelEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::CANCELLED);

            // Then clear program
            EvtClearProgram evt;
            fixture.fsm.Post(evt, EventPriority::UI);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::IDLE);
        }

        TEST_CASE("CANCELLED: EvtError transitions to ERROR")
        {
            FsmTestFixture fixture;
            Profile profile;

            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));
            fixture.Init();

            // Transition to CANCELLED
            EvtLoadProfile loadEvt(profile);
            fixture.fsm.Post(loadEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::LOADED);

            EvtStart startEvt;
            fixture.fsm.Post(startEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::RUNNING);

            EvtCancel cancelEvt;
            fixture.fsm.Post(cancelEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::CANCELLED);

            // Then error
            EvtError evt(Error::SensorFailure, Domain::Furnace, "Test error");
            fixture.fsm.Post(evt, EventPriority::Critical);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::ERROR);
        }
    }
} // namespace HeatTreatFurnace::Test
