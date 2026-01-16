#include <thread>
#include <doctest/doctest.h>
#include <doctest/trompeloeil.hpp>
#include "FsmTestFixture.hpp"

#include <thread>

namespace HeatTreatFurnace::Test
{
    TEST_SUITE("PriorityQueue")
    {
        TEST_CASE("Stores events in correct order")
        {

            EvtManualSetOff off;
            EvtModeManual man;
            EvtModeProfile prof;
            EvtError err(Error::ControllerFailure, Domain::Furnace, "Test error");
            EvtManualSetOn on;

            FsmTestFixture fixture;

            fixture.queueManager.Post(err);
            fixture.queueManager.Post(on);
            fixture.queueManager.Post(err);
            fixture.queueManager.Post(off);
            fixture.queueManager.Post(man);
            fixture.queueManager.Post(prof);
            fixture.queueManager.Post(err);


            // etl::vector<std::chrono::milliseconds, 4> events;

            etl::vector<std::pair<int, EventPriority>, 16> events;

            fixture.queueManager.DrainQueue([&events](etl::imessage & aMsg)
            {
                auto* evt = dynamic_cast<PriorityQueueEvent*>(&aMsg);
                if (!events.full())
                {
                    etl::message_id_t id = evt->get_message_id();
                    events.emplace_back(std::pair<int, EventPriority>(id, evt->priority));
                }

            });

            // INFO(events[0]);
            // INFO(events[1]);
            // INFO(events[2]);
            // INFO(events[3]);
            // INFO(events[4]);
            // INFO(events[5]);

            //critical should come first
INFO("HERE");
        }
    }
}