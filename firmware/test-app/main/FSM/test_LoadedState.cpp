#include <doctest/doctest.h>
#include <doctest/trompeloeil.hpp>
#include "FsmTestFixture.hpp"

namespace HeatTreatFurnace::Test
{
    TEST_SUITE("LoadedState")
    {
        TEST_CASE("LOADED: EvtProfileStart transitions to RUNNING")
        {
            FsmTestFixture fixture;
            Profile profile;
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));
            fixture.Init();

            EvtModeProfile profileEvt;
            fixture.fsm.Post(profileEvt);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE);

            // First transition to LOADED
            EvtProfileLoad loadEvt(profile);
            fixture.fsm.Post(loadEvt);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_LOADED);
        }

        TEST_CASE("LOADED: EvtProfileLoad stays in LOADED")
        {
            FsmTestFixture fixture;
            Profile profile;
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));
            fixture.Init();

            EvtModeProfile profileEvt;
            fixture.fsm.Post(profileEvt);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE);

            // First transition to LOADED
            EvtProfileLoad loadEvt(profile);
            fixture.fsm.Post(loadEvt);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_LOADED);

            // Load it again
            EvtProfileLoad loadEvt2(profile);
            fixture.fsm.Post(loadEvt2);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_LOADED);
        }

        TEST_CASE("LOADED: EvtProfileClear transitions to PROFILE")
        {
            FsmTestFixture fixture;
            Profile profile;
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));
            fixture.Init();

            EvtModeProfile profileEvt;
            fixture.fsm.Post(profileEvt);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE);

            // First transition to LOADED
            EvtProfileLoad loadEvt(profile);
            fixture.fsm.Post(loadEvt);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_LOADED);

            // clear it
            EvtProfileClear clearEvt;
            fixture.fsm.Post(clearEvt);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE);

            REQUIRE(fixture.fsm.GetCurrentProfile().isValid == false);
        }

        TEST_CASE("RUNNING: EvtProfileSetNextSegment remains in Loaded but sets the next segment and segment time")
        {
            FsmTestFixture fixture;
            Profile profile;
            profile.segments.push_back({100.0f, std::chrono::seconds(10), std::chrono::seconds(10)});
            profile.segments.push_back({200.0f, std::chrono::seconds(20), std::chrono::seconds(20)});
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));
            fixture.Init();

            // Load and Start Profile
            fixture.fsm.Post(EvtModeProfile());
            fixture.fsm.Post(EvtProfileLoad(profile));
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_LOADED);

            // Set next segment
            uint16_t nextSegment = 1;
            std::chrono::seconds nextTime(5);
            fixture.fsm.Post(EvtProfileSetNextSegment(nextSegment, nextTime));
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_LOADED);
            REQUIRE(fixture.fsm.GetCurrentProfile().currentSegment == nextSegment);
            REQUIRE((fixture.fsm.GetCurrentProfile().currentSegmentTime == nextTime));
        }

        TEST_CASE("LOADED: EvtError transitions to ERROR")
        {
            FsmTestFixture fixture;
            Profile profile;
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));
            fixture.Init();

            EvtModeProfile profileEvt;
            fixture.fsm.Post(profileEvt);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE);

            // First transition to LOADED
            EvtProfileLoad loadEvt(profile);
            fixture.fsm.Post(loadEvt);
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_LOADED);

            // Then error
            EvtError evt(Error::SafetyInterlock, Domain::Furnace, "Test error");
            fixture.fsm.Post(evt);
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::ERROR);
        }
    }
} // namespace HeatTreatFurnace::Test
