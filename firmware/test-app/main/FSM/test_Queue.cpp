#include <doctest/doctest.h>
#include <doctest/trompeloeil.hpp>
#include "FsmTestFixture.hpp"

namespace HeatTreatFurnace::Test
{
    TEST_SUITE("EventQueue")
    {
        TEST_CASE("Stores events in FIFO order")
        {
            EvtManualSetOff off;
            EvtModeManual man;
            EvtModeProfile prof;
            EvtManualSetOn on;

            FsmTestFixture fixture;

            fixture.queueManager.Post(on);
            fixture.queueManager.Post(off);
            fixture.queueManager.Post(man);
            fixture.queueManager.Post(prof);

            etl::vector<etl::message_id_t, 16> eventIds;

            fixture.queueManager.DrainQueue([&eventIds](etl::imessage& aMsg)
            {
                if (!eventIds.full())
                {
                    eventIds.push_back(aMsg.get_message_id());
                }
            });

            REQUIRE(eventIds.size() == 4);
            REQUIRE(eventIds[0] == EVENT_MANUAL_SET_ON);
            REQUIRE(eventIds[1] == EVENT_MANUAL_SET_OFF);
            REQUIRE(eventIds[2] == EVENT_MODE_MANUAL);
            REQUIRE(eventIds[3] == EVENT_MODE_PROFILE);
        }

        TEST_CASE("Flush clears all events from queue")
        {
            FsmTestFixture fixture;

            fixture.queueManager.Post(EvtModeProfile());
            fixture.queueManager.Post(EvtModeManual());
            fixture.queueManager.Post(EvtManualSetOn());

            fixture.queueManager.Flush();

            etl::vector<etl::message_id_t, 16> eventIds;
            fixture.queueManager.DrainQueue([&eventIds](etl::imessage& aMsg)
            {
                eventIds.push_back(aMsg.get_message_id());
            });

            REQUIRE(eventIds.empty());
        }

        TEST_CASE("Error event flushes queue and transitions immediately")
        {
            FsmTestFixture fixture;

            ALLOW_CALL(fixture.mockLogBackend, WriteLog(trompeloeil::_, trompeloeil::_, trompeloeil::_));

            fixture.Init();
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::OFF);

            fixture.fsm.Post(EvtModeProfile());
            fixture.fsm.Post(EvtModeManual());

            EvtError err(Error::ControllerFailure, Domain::Furnace, "Test error");
            fixture.fsm.Post(err);

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::ERROR);

            etl::vector<etl::message_id_t, 16> eventIds;
            fixture.queueManager.DrainQueue([&eventIds](etl::imessage& aMsg)
            {
                eventIds.push_back(aMsg.get_message_id());
            });

            REQUIRE(eventIds.empty());
        }
    }
}
