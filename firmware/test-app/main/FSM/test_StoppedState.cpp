#include <doctest/doctest.h>
#include <doctest/trompeloeil.hpp>
#include "FsmTestFixture.hpp"

namespace HeatTreatFurnace::Test
{
    TEST_SUITE("StoppedState")
    {
        TEST_CASE("STOPPED: Transitions to OFF")
        {
            FsmTestFixture fixture;
            Profile profile;
            profile.segments.push_back({100.0f, std::chrono::seconds(10), std::chrono::seconds(10)});
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));
            fixture.Init();

            // Transition to STOPPED
            fixture.fsm.Post(EvtModeProfile());
            fixture.fsm.Post(EvtProfileLoad(profile));
            fixture.fsm.Post(EvtProfileStart());
            fixture.fsm.Post(EvtProfileStop());
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_STOPPED);

            fixture.fsm.Post(EvtModeOff());
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::OFF);
        }

        TEST_CASE("STOPPED: Transitions to MANUAL")
        {
            FsmTestFixture fixture;
            Profile profile;
            profile.segments.push_back({100.0f, std::chrono::seconds(10), std::chrono::seconds(10)});
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));
            fixture.Init();

            // Transition to STOPPED
            fixture.fsm.Post(EvtModeProfile());
            fixture.fsm.Post(EvtProfileLoad(profile));
            fixture.fsm.Post(EvtProfileStart());
            fixture.fsm.Post(EvtProfileStop());
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_STOPPED);

            fixture.fsm.Post(EvtModeManual());
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::MANUAL);
        }

        TEST_CASE("STOPPED: EvtProfileLoad transitions to PROFILE_LOADED")
        {
            FsmTestFixture fixture;
            Profile profile;
            profile.segments.push_back({100.0f, std::chrono::seconds(10), std::chrono::seconds(10)});
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));
            fixture.Init();

            // Transition to STOPPED
            fixture.fsm.Post(EvtModeProfile());
            fixture.fsm.Post(EvtProfileLoad(profile));
            fixture.fsm.Post(EvtProfileStart());
            fixture.fsm.Post(EvtProfileStop());
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_STOPPED);

            Profile newProfile;
            newProfile.name = "New Profile";
            fixture.fsm.Post(EvtProfileLoad(newProfile));
            fixture.fsm.ProcessQueue();

            // In ProfileStoppedState.cpp, EvtProfileLoad transitions to STATE_PROFILE
            // then ProfileState::on_enter_state posts EvtProfileAlreadyLoaded if a profile is set,
            // which transitions to STATE_PROFILE_LOADED.
            fixture.fsm.ProcessQueue(); 

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_LOADED);
            REQUIRE(fixture.fsm.GetCurrentProfile().name == "New Profile");
        }

        TEST_CASE("STOPPED: EvtStart transitions to RUNNING when RUNNING profile is stopped and resumed")
        {
            FsmTestFixture fixture;
            Profile profile;
            profile.segments.push_back({100.0f, std::chrono::seconds(10), std::chrono::seconds(10)});
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));
            fixture.Init();

            // Transition to STOPPED
            fixture.fsm.Post(EvtModeProfile());
            fixture.fsm.Post(EvtProfileLoad(profile));
            fixture.fsm.Post(EvtProfileStart());
            fixture.fsm.Post(EvtProfileStop());
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_STOPPED);

            fixture.fsm.Post(EvtProfileStart());
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_RUNNING);
        }

        TEST_CASE("PAUSED: EvtProfileClear transitions to PROFILE")
        {
            FsmTestFixture fixture;
            Profile profile;
            profile.segments.push_back({100.0f, std::chrono::seconds(10), std::chrono::seconds(10)});
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));
            fixture.Init();

            // Transition to STOPPED
            fixture.fsm.Post(EvtModeProfile());
            fixture.fsm.Post(EvtProfileLoad(profile));
            fixture.fsm.Post(EvtProfileStart());
            fixture.fsm.Post(EvtProfileStop());
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_STOPPED);

            fixture.fsm.Post(EvtProfileClear());
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE);
            REQUIRE(fixture.fsm.GetCurrentProfile().isValid == false);
        }

        TEST_CASE("PAUSED: EvtError transitions to ERROR")
        {
            FsmTestFixture fixture;
            Profile profile;
            profile.segments.push_back({100.0f, std::chrono::seconds(10), std::chrono::seconds(10)});
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_,_,_));
            fixture.Init();

            // Transition to STOPPED
            fixture.fsm.Post(EvtModeProfile());
            fixture.fsm.Post(EvtProfileLoad(profile));
            fixture.fsm.Post(EvtProfileStart());
            fixture.fsm.Post(EvtProfileStop());
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_STOPPED);

            fixture.fsm.Post(EvtError(Error::SafetyInterlock, Domain::Furnace, "Test error"));
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::ERROR);
        }
    }
} // namespace HeatTreatFurnace::Test
