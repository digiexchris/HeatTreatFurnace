#include <doctest/doctest.h>

#include "../../../lib/HeatTreatFurnace/Pid/PidController.hpp"
#include "../../../lib/HeatTreatFurnace/Pid/PidConfig.hpp"

namespace HeatTreatFurnace::Test
{
    TEST_SUITE("PidController")
    {
        TEST_CASE("Compute uses proportional and integral terms")
        {
            Pid::PidConfig config;
            config.kp = 2.0f;
            config.ki = 1.0f;
            config.kd = 0.0f;
            config.outputMin = -1000.0f;
            config.outputMax = 1000.0f;
            config.integralMin = -1000.0f;
            config.integralMax = 1000.0f;
            config.sampleTimeMs = 1000.0f;

            Pid::PidController pid(config);
            float output = pid.Compute(10.0f, 8.0f);
            CHECK(output == doctest::Approx(6.0f));
        }

        TEST_CASE("Output is clamped to limits")
        {
            Pid::PidConfig config;
            config.kp = 10.0f;
            config.ki = 0.0f;
            config.kd = 0.0f;
            config.outputMin = -5.0f;
            config.outputMax = 5.0f;

            Pid::PidController pid(config);
            float output = pid.Compute(10.0f, 0.0f);
            CHECK(output == doctest::Approx(5.0f));
        }

        TEST_CASE("Integral term is clamped")
        {
            Pid::PidConfig config;
            config.kp = 0.0f;
            config.ki = 1.0f;
            config.kd = 0.0f;
            config.integralMin = -1.0f;
            config.integralMax = 1.0f;
            config.outputMin = -10.0f;
            config.outputMax = 10.0f;
            config.sampleTimeMs = 1000.0f;

            Pid::PidController pid(config);
            float outputFirst = pid.Compute(10.0f, 0.0f);
            float outputSecond = pid.Compute(10.0f, 0.0f);
            CHECK(outputFirst == doctest::Approx(1.0f));
            CHECK(outputSecond == doctest::Approx(1.0f));
        }

        TEST_CASE("Derivative filtering smooths output")
        {
            Pid::PidConfig config;
            config.kp = 0.0f;
            config.ki = 0.0f;
            config.kd = 1.0f;
            config.derivativeFilterCoeff = 0.5f;
            config.outputMin = -1000.0f;
            config.outputMax = 1000.0f;
            config.sampleTimeMs = 1000.0f;

            Pid::PidController pid(config);
            pid.Compute(100.0f, 0.0f);
            float output1 = pid.Compute(100.0f, 10.0f);
            float output2 = pid.Compute(100.0f, 10.0f);
            CHECK(output1 == doctest::Approx(-5.0f));
            CHECK(output2 == doctest::Approx(-2.5f));
        }

        TEST_CASE("Reset clears integral and previous state")
        {
            Pid::PidConfig config;
            config.kp = 0.0f;
            config.ki = 1.0f;
            config.kd = 0.0f;
            config.integralMin = -1000.0f;
            config.integralMax = 1000.0f;
            config.outputMin = -1000.0f;
            config.outputMax = 1000.0f;
            config.sampleTimeMs = 1000.0f;

            Pid::PidController pid(config);
            pid.Compute(10.0f, 0.0f);
            pid.Compute(10.0f, 0.0f);
            pid.Reset();
            float outputAfterReset = pid.Compute(10.0f, 0.0f);
            CHECK(outputAfterReset == doctest::Approx(10.0f));
        }

        TEST_CASE("SetTunings changes PID gains")
        {
            Pid::PidConfig config;
            config.kp = 1.0f;
            config.ki = 0.0f;
            config.kd = 0.0f;
            config.outputMin = -1000.0f;
            config.outputMax = 1000.0f;

            Pid::PidController pid(config);
            float outputBefore = pid.Compute(10.0f, 0.0f);
            CHECK(outputBefore == doctest::Approx(10.0f));

            pid.SetTunings(2.0f, 0.0f, 0.0f);
            pid.Reset();
            float outputAfter = pid.Compute(10.0f, 0.0f);
            CHECK(outputAfter == doctest::Approx(20.0f));
        }

        TEST_CASE("SetOutputLimits changes output clamping")
        {
            Pid::PidConfig config;
            config.kp = 10.0f;
            config.ki = 0.0f;
            config.kd = 0.0f;
            config.outputMin = 0.0f;
            config.outputMax = 100.0f;

            Pid::PidController pid(config);
            float outputBefore = pid.Compute(10.0f, 0.0f);
            CHECK(outputBefore == doctest::Approx(100.0f));

            pid.SetOutputLimits(0.0f, 50.0f);
            pid.Reset();
            float outputAfter = pid.Compute(10.0f, 0.0f);
            CHECK(outputAfter == doctest::Approx(50.0f));
        }

        TEST_CASE("SetIntegralLimits changes integral clamping")
        {
            Pid::PidConfig config;
            config.kp = 0.0f;
            config.ki = 1.0f;
            config.kd = 0.0f;
            config.integralMin = -100.0f;
            config.integralMax = 100.0f;
            config.outputMin = -1000.0f;
            config.outputMax = 1000.0f;
            config.sampleTimeMs = 1000.0f;

            Pid::PidController pid(config);
            pid.Compute(50.0f, 0.0f);
            pid.Compute(50.0f, 0.0f);
            pid.Compute(50.0f, 0.0f);
            float outputBefore = pid.Compute(50.0f, 0.0f);
            CHECK(outputBefore == doctest::Approx(100.0f));

            pid.SetIntegralLimits(-10.0f, 10.0f);
            pid.Reset();
            pid.Compute(50.0f, 0.0f);
            float outputAfter = pid.Compute(50.0f, 0.0f);
            CHECK(outputAfter == doctest::Approx(10.0f));
        }
    }
}
