#include <doctest/doctest.h>
#include <doctest/trompeloeil.hpp>
#include "../../../lib/HeatTreatFurnace/Heater/HeaterController.hpp"
#include "Log/LogService.hpp"
#include "mocks/LogBackend.hpp"

namespace HeatTreatFurnace::Test
{
    TEST_SUITE("HeaterController")
    {
        TEST_CASE("Disabled controller outputs zero")
        {
            MockLogBackend mockLogBackend(Log::LogLevel::None);
            Log::LogService logger(&mockLogBackend);
            Heater::HeaterController heater(logger);

            REQUIRE_CALL(mockLogBackend, WriteLog(trompeloeil::_, "HeaterController", R"(Target temperature set to 100.0)")).TIMES(1);

            heater.SetTargetTemp(100.0f);
            float output = heater.Update(20.0f);
            CHECK(output == doctest::Approx(0.0f));
        }

        TEST_CASE("Enabled controller computes output and disables to zero")
        {
            MockLogBackend mockLogBackend(Log::LogLevel::None);
            Log::LogService logger(&mockLogBackend);
            Heater::HeaterController heater(logger);

            REQUIRE_CALL(mockLogBackend, WriteLog(trompeloeil::_, "HeaterController", R"(Heater enabled)")).TIMES(1);
            REQUIRE_CALL(mockLogBackend, WriteLog(trompeloeil::_, "HeaterController", R"(Target temperature set to 30.0)")).TIMES(1);

            heater.Enable();
            heater.SetTargetTemp(30.0f);
            float output = heater.Update(20.0f);
            CHECK(output == doctest::Approx(20.1f));

            REQUIRE_CALL(mockLogBackend, WriteLog(trompeloeil::_, "HeaterController", R"(Heater disabled)")).TIMES(1);
            heater.Disable();
            CHECK(heater.Update(20.0f) == doctest::Approx(0.0f));
        }
    }
}
