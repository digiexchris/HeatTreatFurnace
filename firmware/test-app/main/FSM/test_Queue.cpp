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

        TEST_CASE("Queue overflow increments overflow count and returns false")
        {
            FsmTestFixture fixture;

            REQUIRE(fixture.queueManager.GetOverflowCount() == 0);

            for (size_t i = 0; i < 48; ++i)
            {
                bool result = fixture.queueManager.Post(EvtTick());
                REQUIRE(result == true);
            }

            REQUIRE(fixture.queueManager.GetOverflowCount() == 0);

            auto containsString = [](const std::string& expected)
            {
                return trompeloeil::make_matcher<etl::string_view>(
                    [expected](etl::string_view msg)
                    {
                        return std::string(msg.data(), msg.size()).find(expected) != std::string::npos;
                    },
                    [expected](std::ostream& os)
                    {
                        os << " containing \"" << expected << "\"";
                    }
                );
            };

            {
                REQUIRE_CALL(fixture.mockLogBackend, WriteLog(
                                 LogLevel::Warn,
                                 containsString("QUEUE"),
                                 containsString("EVENT_MODE_PROFILE")
                             )).TIMES(1);

                bool overflowResult = fixture.queueManager.Post(EvtModeProfile());
                REQUIRE(overflowResult == false);
                REQUIRE(fixture.queueManager.GetOverflowCount() == 1);
            }

            {
                REQUIRE_CALL(fixture.mockLogBackend, WriteLog(
                                 LogLevel::Warn,
                                 containsString("QUEUE"),
                                 containsString("EVENT_MODE_MANUAL")
                             )).TIMES(1);

                bool overflowResult = fixture.queueManager.Post(EvtModeManual());
                REQUIRE(overflowResult == false);
                REQUIRE(fixture.queueManager.GetOverflowCount() == 2);
            }

            {
                REQUIRE_CALL(fixture.mockLogBackend, WriteLog(
                                 LogLevel::Warn,
                                 containsString("QUEUE"),
                                 containsString("EVENT_MODE_OFF")
                             )).TIMES(1);

                bool overflowResult = fixture.queueManager.Post(EvtModeOff());
                REQUIRE(overflowResult == false);
                REQUIRE(fixture.queueManager.GetOverflowCount() == 3);
            }

            fixture.queueManager.ResetOverflowCount();
            REQUIRE(fixture.queueManager.GetOverflowCount() == 0);
        }
    }
}
