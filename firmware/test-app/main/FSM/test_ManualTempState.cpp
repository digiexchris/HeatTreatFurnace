#include <doctest/doctest.h>
#include <doctest/trompeloeil.hpp>
#include "FsmTestFixture.hpp"

namespace HeatTreatFurnace::Test
{
    TEST_SUITE("ManualTempState")
    {
        TEST_CASE("MANUAL_TEMP: EvtResume transitions to IDLE when no profile loaded")
        {
            FsmTestFixture fixture;

            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1);
            fixture.Init();

            // Transition to MANUAL_TEMP from IDLE
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("IdleState"),_)).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("IdleState"),etl::string_view("Exiting IDLE state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ManualTempState"),etl::string_view("Entered MANUAL_TEMP state"))).TIMES(1);
            EvtSetManualTemp manualEvt(100.0f);
            fixture.fsm.Post(manualEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            // Then resume
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ManualTempState"),etl::string_view("Received EvtResume"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ManualTempState"),etl::string_view("Disabling manual control, returning to IDLE"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ManualTempState"),etl::string_view("Exiting MANUAL_TEMP state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("IdleState"),etl::string_view("Entered IDLE state"))).TIMES(1);
            EvtResume evt;
            fixture.fsm.Post(evt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::IDLE);
        }

        TEST_CASE("MANUAL_TEMP: EvtSetManualTemp stays in MANUAL_TEMP")
        {
            FsmTestFixture fixture;

            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1);
            fixture.Init();

            // Transition to MANUAL_TEMP
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("IdleState"),etl::string_view("Exiting IDLE state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ManualTempState"),etl::string_view("Entered MANUAL_TEMP state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_,etl::string_view("IdleState"),etl::string_view("Manual temperature requested, transitioning to MANUAL_TEMP")));
            EvtSetManualTemp manualEvt1(100.0f);
            fixture.fsm.Post(manualEvt1, EventPriority::UI);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::MANUAL_TEMP);

            // Then update manual temp
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ManualTempState"),_)).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ManualTempState"),etl::string_view("Manual temperature updated"))).TIMES(1);
            EvtSetManualTemp manualEvt2(150.0f);
            fixture.fsm.Post(manualEvt2, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::MANUAL_TEMP);
        }

        TEST_CASE("MANUAL_TEMP: EvtError transitions to ERROR")
        {
            FsmTestFixture fixture;

            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1);
            fixture.Init();

            // Transition to MANUAL_TEMP
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("IdleState"),etl::string_view("Exiting IDLE state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ManualTempState"),etl::string_view("Entered MANUAL_TEMP state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_,etl::string_view("IdleState"),etl::string_view("Manual temperature requested, transitioning to MANUAL_TEMP")));
            EvtSetManualTemp manualEvt(100.0f);
            fixture.fsm.Post(manualEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            // Then error
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ManualTempState"),etl::string_view("Received EvtError: aMessage"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ManualTempState"),etl::string_view("Exiting MANUAL_TEMP state"))).TIMES(1);
            REQUIRE_CALL(fixture.mockLogBackend,
                         WriteLog(_, etl::string_view("ErrorState"),etl::string_view("Entered ERROR state"))).TIMES(1);
            EvtError evt(Error::Unknown, Domain::StateMachine, "aMessage");
            fixture.fsm.Post(evt, EventPriority::Critical);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::ERROR);
        }
    }
} // namespace HeatTreatFurnace::Test
