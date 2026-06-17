#include "ThermalSimulator.hpp"
#include <algorithm>

namespace Simulator
{
    namespace
    {
        HeatTreatFurnace::Pid::PidConfig CreatePidConfig()
        {
            HeatTreatFurnace::Pid::PidConfig config;
            config.kp = Config::PID_KP;
            config.ki = Config::PID_KI;
            config.kd = Config::PID_KD;
            config.outputMin = Config::PID_OUTPUT_MIN;
            config.outputMax = Config::PID_OUTPUT_MAX;
            config.sampleTimeMs = static_cast<float>(Config::TICK_INTERVAL_MS);
            return config;
        }
    }

    ThermalSimulator::ThermalSimulator()
        : myPid(CreatePidConfig())
        , myTargetTemp(0.0f)
        , myKilnTemp(Config::AMBIENT_TEMP)
        , myCaseTemp(Config::AMBIENT_TEMP)
        , myHeatOutput(0.0f)
        , myTempChangeRate(0.0f)
        , myTimeScale(Config::DEFAULT_TIME_SCALE)
    {
    }

    bool ThermalSimulator::SetTargetTemp(float aTemp)
    {
        myTargetTemp = aTemp;
        return true;
    }

    float ThermalSimulator::Update(float aCurrentTemp)
    {
        if (!myIsEnabled)
        {
            myHeatOutput = 0.0f;
            return 0.0f;
        }

        myHeatOutput = myPid.Compute(myTargetTemp, aCurrentTemp);
        return myHeatOutput;
    }

    float ThermalSimulator::GetOutput() const
    {
        return myHeatOutput;
    }

    float ThermalSimulator::GetTargetTemp() const
    {
        return myTargetTemp;
    }

    void ThermalSimulator::SetPidTunings(float aKp, float aKi, float aKd)
    {
        myPid.SetTunings(aKp, aKi, aKd);
    }

    void ThermalSimulator::SetPidOutputLimits(float aMin, float aMax)
    {
        myPid.SetOutputLimits(aMin, aMax);
    }

    void ThermalSimulator::UpdateSimulation(float aDeltaSeconds)
    {
        float scaledDelta = aDeltaSeconds * myTimeScale;
        float previousTemp = myKilnTemp;

        // Heat input from heater (scaled by output percentage)
        float heatInput = 0.0f;
        if (myIsEnabled)
        {
            // Update PID to get heat output
            myHeatOutput = myPid.Compute(myTargetTemp, myKilnTemp);
            heatInput = (myHeatOutput / 100.0f) * Config::HEATER_POWER * scaledDelta;
        }
        else
        {
            myHeatOutput = 0.0f;
        }

        // Heat loss due to cooling (Newton's law of cooling)
        float tempDiff = myKilnTemp - Config::AMBIENT_TEMP;
        float heatLoss = Config::COOLING_COEFFICIENT * tempDiff * Config::THERMAL_MASS * scaledDelta;

        // Apply thermal model
        float netHeat = heatInput - heatLoss;
        myKilnTemp += netHeat / Config::THERMAL_MASS * Config::THERMAL_MASS; // Simplified: just netHeat since thermal mass cancels

        // Ensure temperature doesn't go below ambient
        myKilnTemp = std::max(myKilnTemp, Config::AMBIENT_TEMP);

        // Calculate temperature change rate (degC/sec in simulated time)
        if (scaledDelta > 0.0f)
        {
            myTempChangeRate = (myKilnTemp - previousTemp) / scaledDelta;
        }

        // Case temperature is a fraction of kiln temperature above ambient
        myCaseTemp = Config::AMBIENT_TEMP + (myKilnTemp - Config::AMBIENT_TEMP) * Config::CASE_TEMP_FACTOR;
    }

    void ThermalSimulator::SetTimeScale(float aScale)
    {
        myTimeScale = std::clamp(aScale, Config::MIN_TIME_SCALE, Config::MAX_TIME_SCALE);
    }
} // namespace Simulator
