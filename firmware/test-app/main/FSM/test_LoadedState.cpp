#include <doctest/doctest.h>
#include <doctest/trompeloeil.hpp>
#include "FsmTestFixture.hpp"

namespace HeatTreatFurnace::Test
{
    TEST_SUITE("LoadedState")
    {
        TEST_CASE("LOADED: EvtStart transitions to RUNNING")
        {
            FsmTestFixture fixture;
            Profile profile;
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1);
            fixture.Init();

            // First transition to LOADED
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("IdleState"), etl::string_view("Profile loaded, transitioning to LOADED"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("IdleState"), etl::string_view("Exiting IDLE state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"), etl::string_view("Entered LOADED state"))).TIMES(1);
            EvtLoadProfile loadEvt(profile);
            fixture.fsm.Post(loadEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            // Then test EvtStart
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"), etl::string_view("Received EvtStart"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"), etl::string_view("Starting program execution"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"), etl::string_view("Exiting LOADED state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("RunningState"), etl::string_view("Entered RUNNING state"))).TIMES(1);
            EvtStart evt;
            fixture.fsm.Post(evt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::RUNNING);
        }

        TEST_CASE("LOADED: EvtLoadProfile stays in LOADED")
        {
            FsmTestFixture fixture;
            Profile profile;
            Profile profile2;
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1);
            fixture.Init();

            // First transition to LOADED
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("IdleState"), etl::string_view("Profile loaded, transitioning to LOADED"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("IdleState"), etl::string_view("Exiting IDLE state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"), etl::string_view("Entered LOADED state"))).TIMES(1);
            EvtLoadProfile loadEvt1(profile);
            fixture.fsm.Post(loadEvt1, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            // Load another profile
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"), etl::string_view("Received EvtLoadProfile, replacing current profile"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"), etl::string_view("Profile reloaded"))).TIMES(1);
            EvtLoadProfile loadEvt2(profile2);
            fixture.fsm.Post(loadEvt2, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::LOADED);
        }

        TEST_CASE("LOADED: EvtClearProgram transitions to IDLE")
        {
            FsmTestFixture fixture;
            Profile profile;
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1);
            fixture.Init();

            // First transition to LOADED
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("IdleState"), etl::string_view("Profile loaded, transitioning to LOADED"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("IdleState"), etl::string_view("Exiting IDLE state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"), etl::string_view("Entered LOADED state"))).TIMES(1);
            EvtLoadProfile loadEvt(profile);
            fixture.fsm.Post(loadEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            // Then clear program
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"), etl::string_view("Received EvtClearProgram"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"), etl::string_view("Program cleared, returning to IDLE"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"), etl::string_view("Exiting LOADED state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("IdleState"), etl::string_view("Entered IDLE state"))).TIMES(1);
            EvtClearProgram evt;
            fixture.fsm.Post(evt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::IDLE);
        }

        TEST_CASE("LOADED: EvtSetManualTemp transitions to MANUAL_TEMP")
        {
            FsmTestFixture fixture;
            Profile profile;
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1);
            fixture.Init();

            // First transition to LOADED
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("IdleState"), etl::string_view("Profile loaded, transitioning to LOADED"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("IdleState"), etl::string_view("Exiting IDLE state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"), etl::string_view("Entered LOADED state"))).TIMES(1);
            EvtLoadProfile loadEvt(profile);
            fixture.fsm.Post(loadEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            // Then set manual temp
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"), etl::string_view("Received EvtSetManualTemp, target: 100 C"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"), etl::string_view("Transitioning to MANUAL_TEMP mode"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"), etl::string_view("Exiting LOADED state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ManualTempState"), etl::string_view("Entered MANUAL_TEMP state"))).TIMES(1);
            EvtSetManualTemp evt(100.0f);
            fixture.fsm.Post(evt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::MANUAL_TEMP);
        }

        TEST_CASE("LOADED: EvtError transitions to ERROR")
        {
            FsmTestFixture fixture;
            Profile profile;
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1);
            fixture.Init();

            // First transition to LOADED
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("IdleState"), etl::string_view("Profile loaded, transitioning to LOADED"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("IdleState"), etl::string_view("Exiting IDLE state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"), etl::string_view("Entered LOADED state"))).TIMES(1);
            EvtLoadProfile loadEvt(profile);
            fixture.fsm.Post(loadEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            // Then error
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"), etl::string_view("Received EvtError: Test error"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("LoadedState"), etl::string_view("Exiting LOADED state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ErrorState"), etl::string_view("Entered ERROR state"))).TIMES(1);
            EvtError evt(Error::SafetyInterlock, Domain::Furnace, "Test error");
            fixture.fsm.Post(evt, EventPriority::Critical);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::ERROR);
        }
    }
} // namespace HeatTreatFurnace::Test
