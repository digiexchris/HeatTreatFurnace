#include <doctest/doctest.h>
#include <doctest/trompeloeil.hpp>

#include "FsmTestFixture.hpp"

namespace HeatTreatFurnace::Test
{
    TEST_SUITE("RunningState")
    {
        TEST_CASE("RUNNING: Transitions to OFF")
        {
            FsmTestFixture fixture;
            Profile profile;
            profile.segments.push_back({100.0f, std::chrono::seconds(10), std::chrono::seconds(10)});
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));

            fixture.Init();

            fixture.fsm.Post(EvtModeProfile());
            fixture.fsm.Post(EvtProfileLoad(profile));
            fixture.fsm.Post(EvtProfileStart());
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_RUNNING);
            REQUIRE(fixture.mockHeater.IsEnabled());

            fixture.fsm.Post(EvtModeOff());
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::OFF);
            REQUIRE(!fixture.mockHeater.IsEnabled());
        }

        TEST_CASE("RUNNING: Transitions to Manual")
        {
            FsmTestFixture fixture;
            Profile profile;
            profile.segments.push_back({100.0f, std::chrono::seconds(10), std::chrono::seconds(10)});
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));

            fixture.Init();

            fixture.fsm.Post(EvtModeProfile());
            fixture.fsm.Post(EvtProfileLoad(profile));
            fixture.fsm.Post(EvtProfileStart());
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_RUNNING);
            REQUIRE(fixture.mockHeater.IsEnabled());

            fixture.fsm.Post(EvtModeManual());
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::MANUAL_OFF);
            REQUIRE(!fixture.mockHeater.IsEnabled());
        }

        TEST_CASE("RUNNING: EvtProfileStop transitions to Stopped")
        {
            FsmTestFixture fixture;
            Profile profile;
            profile.segments.push_back({100.0f, std::chrono::seconds(10), std::chrono::seconds(10)});
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));

            fixture.Init();

            fixture.fsm.Post(EvtModeProfile());
            fixture.fsm.Post(EvtProfileLoad(profile));
            fixture.fsm.Post(EvtProfileStart());
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_RUNNING);
            REQUIRE(fixture.mockHeater.IsEnabled());

            fixture.fsm.Post(EvtProfileStop());
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_STOPPED);
            REQUIRE(!fixture.mockHeater.IsEnabled());
        }

        TEST_CASE("RUNNING: EvtProfileComplete transitions to Completed")
        {
            FsmTestFixture fixture;
            Profile profile;
            profile.segments.push_back({100.0f, std::chrono::seconds(10), std::chrono::seconds(10)});
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));

            fixture.Init();

            fixture.fsm.Post(EvtModeProfile());
            fixture.fsm.Post(EvtProfileLoad(profile));
            fixture.fsm.Post(EvtProfileStart());
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_RUNNING);
            REQUIRE(fixture.mockHeater.IsEnabled());

            fixture.fsm.Post(EvtProfileComplete());
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_COMPLETED);
            REQUIRE(fixture.fsm.GetCurrentProfile().runCompleted == true);
            REQUIRE(!fixture.mockHeater.IsEnabled());
        }

        TEST_CASE("RUNNING: EvtProfileSetNextSegment remains in Running but sets the next segment and segment time")
        {
            FsmTestFixture fixture;
            Profile profile;
            profile.segments.push_back({100.0f, std::chrono::seconds(10), std::chrono::seconds(10)});
            profile.segments.push_back({200.0f, std::chrono::seconds(20), std::chrono::seconds(20)});
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));

            fixture.Init();

            fixture.fsm.Post(EvtModeProfile());
            fixture.fsm.Post(EvtProfileLoad(profile));
            fixture.fsm.Post(EvtProfileStart());
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_RUNNING);
            REQUIRE(fixture.mockHeater.IsEnabled());

            uint16_t nextSegment = 1;
            std::chrono::seconds nextTime(5);
            fixture.fsm.Post(EvtProfileSetNextSegment(nextSegment, nextTime));
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_RUNNING);
            REQUIRE(fixture.fsm.GetCurrentProfile().currentSegment == nextSegment);
            REQUIRE((fixture.fsm.GetCurrentProfile().currentSegmentTime == nextTime));
            REQUIRE(fixture.mockHeater.IsEnabled());
        }

        TEST_CASE("RUNNING: EvtError transitions to ERROR")
        {
            FsmTestFixture fixture;
            Profile profile;
            profile.segments.push_back({100.0f, std::chrono::seconds(10), std::chrono::seconds(10)});
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));

            fixture.Init();

            fixture.fsm.Post(EvtModeProfile());
            fixture.fsm.Post(EvtProfileLoad(profile));
            fixture.fsm.Post(EvtProfileStart());
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_RUNNING);
            REQUIRE(fixture.mockHeater.IsEnabled());

            fixture.fsm.Post(EvtError(Error::SafetyInterlock, Domain::Furnace, "Test error"));
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::ERROR);
            REQUIRE(!fixture.mockHeater.IsEnabled());
        }
    }
} // namespace HeatTreatFurnace::Test
