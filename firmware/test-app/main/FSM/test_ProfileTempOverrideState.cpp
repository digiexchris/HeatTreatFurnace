#include <doctest/doctest.h>
#include <doctest/trompeloeil.hpp>
#include "FsmTestFixture.hpp"

namespace HeatTreatFurnace::Test
{
    TEST_SUITE("ProfileTempOverrideState")
    {
        TEST_CASE("PROFILE_TEMP_OVERRIDE: EvtResume transitions to RUNNING")
        {
            FsmTestFixture fixture;
            Profile profile;

            // Initialize fixture
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1);
            fixture.Init();

            // Transition IDLE -> LOADED
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Exiting IDLE state"
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Profile loaded, transitioning to LOADED"
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Entered LOADED state"
            EvtProfileLoad loadEvt(profile);
            fixture.fsm.Post(loadEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            // Transition LOADED -> RUNNING
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Received EvtProfileStart"
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Starting program execution"
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Exiting LOADED state"
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Entered RUNNING state"
            EvtProfileStart startEvt;
            fixture.fsm.Post(startEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            // Transition RUNNING -> PROFILE_TEMP_OVERRIDE
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Received EvtManualSetTemp, target: {} C"
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Transitioning to PROFILE_TEMP_OVERRIDE mode"
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Exiting RUNNING state"
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Entered PROFILE_TEMP_OVERRIDE state"
            EvtManualSetTemp manualEvt(150.0f);
            fixture.fsm.Post(manualEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            // Transition PROFILE_TEMP_OVERRIDE -> RUNNING
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Received EvtResume"
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Override cleared, resuming profile control"
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Exiting PROFILE_TEMP_OVERRIDE state"
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Entered RUNNING state"
            EvtResume evt;
            fixture.fsm.Post(evt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::RUNNING);
        }

        TEST_CASE("PROFILE_TEMP_OVERRIDE: EvtManualSetTemp stays in PROFILE_TEMP_OVERRIDE")
        {
            FsmTestFixture fixture;
            Profile profile;

            // Initialize fixture
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1);
            fixture.Init();

            // Transition IDLE -> LOADED
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Exiting IDLE state"
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Profile loaded, transitioning to LOADED"
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Entered LOADED state"
            EvtProfileLoad loadEvt(profile);
            fixture.fsm.Post(loadEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            // Transition LOADED -> RUNNING
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Received EvtProfileStart"
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Starting program execution"
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Exiting LOADED state"
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Entered RUNNING state"
            EvtProfileStart startEvt;
            fixture.fsm.Post(startEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            // Transition RUNNING -> PROFILE_TEMP_OVERRIDE
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Received EvtManualSetTemp, target: {} C"
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Transitioning to PROFILE_TEMP_OVERRIDE mode"
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Exiting RUNNING state"
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Entered PROFILE_TEMP_OVERRIDE state"
            EvtManualSetTemp manualEvt1(150.0f);
            fixture.fsm.Post(manualEvt1, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            // Stay in PROFILE_TEMP_OVERRIDE with new temp
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Received EvtManualSetTemp, new target: {} C"
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Override temperature updated"
            EvtManualSetTemp manualEvt2(200.0f);
            fixture.fsm.Post(manualEvt2, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_TEMP_OVERRIDE);
        }

        TEST_CASE("PROFILE_TEMP_OVERRIDE: EvtProfileStop transitions to CANCELLED")
        {
            FsmTestFixture fixture;
            Profile profile;

            // Initialize fixture
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1);
            fixture.Init();

            // Transition IDLE -> LOADED
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Exiting IDLE state"
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Profile loaded, transitioning to LOADED"
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Entered LOADED state"
            EvtProfileLoad loadEvt(profile);
            fixture.fsm.Post(loadEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            // Transition LOADED -> RUNNING
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Received EvtProfileStart"
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Starting program execution"
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Exiting LOADED state"
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Entered RUNNING state"
            EvtProfileStart startEvt;
            fixture.fsm.Post(startEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            // Transition RUNNING -> PROFILE_TEMP_OVERRIDE
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Received EvtManualSetTemp, target: {} C"
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Transitioning to PROFILE_TEMP_OVERRIDE mode"
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Exiting RUNNING state"
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Entered PROFILE_TEMP_OVERRIDE state"
            EvtManualSetTemp manualEvt(150.0f);
            fixture.fsm.Post(manualEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            // Transition PROFILE_TEMP_OVERRIDE -> CANCELLED
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Received EvtProfileStop"
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Cancelling program execution"
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Exiting PROFILE_TEMP_OVERRIDE state"
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Entered CANCELLED state"
            EvtProfileStop evt;
            fixture.fsm.Post(evt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::CANCELLED);
        }

        TEST_CASE("PROFILE_TEMP_OVERRIDE: EvtError transitions to ERROR")
        {
            FsmTestFixture fixture;
            Profile profile;

            // Initialize fixture
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1);
            fixture.Init();

            // Transition IDLE -> LOADED
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Exiting IDLE state"
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Profile loaded, transitioning to LOADED"
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Entered LOADED state"
            EvtProfileLoad loadEvt(profile);
            fixture.fsm.Post(loadEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            // Transition LOADED -> RUNNING
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Received EvtProfileStart"
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Starting program execution"
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Exiting LOADED state"
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Entered RUNNING state"
            EvtProfileStart startEvt;
            fixture.fsm.Post(startEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            // Transition RUNNING -> PROFILE_TEMP_OVERRIDE
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Received EvtManualSetTemp, target: {} C"
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Transitioning to PROFILE_TEMP_OVERRIDE mode"
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Exiting RUNNING state"
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Entered PROFILE_TEMP_OVERRIDE state"
            EvtManualSetTemp manualEvt(150.0f);
            fixture.fsm.Post(manualEvt, EventPriority::UI);
            fixture.fsm.ProcessQueue();

            // Transition PROFILE_TEMP_OVERRIDE -> ERROR
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Received EvtError: {}"
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Exiting PROFILE_TEMP_OVERRIDE state"
            REQUIRE_CALL(fixture.mockLogBackend, WriteLog(_,_,_)).TIMES(1); // "Entered ERROR state"
            EvtError evt(Error::SensorFailure, Domain::Furnace, "Test error");
            fixture.fsm.Post(evt, EventPriority::Critical);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::ERROR);
        }
    }
} // namespace HeatTreatFurnace::Test
