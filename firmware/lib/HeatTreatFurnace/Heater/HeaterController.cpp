#include "Heater/HeaterController.hpp"

namespace HeatTreatFurnace::Heater
{
    HeaterController::HeaterController(Log::LogService& aLogger)
        : myPid(Pid::PidConfig{}),
          myLogger(aLogger)
    {
    }

    bool HeaterController::Enable()
    {
        IHeaterController::Enable();
        myLogger.Log(Log::LogLevel::Info, "HeaterController", "Heater enabled");
        return true;
    }

    bool HeaterController::Disable()
    {
        IHeaterController::Disable();
        myOutput = 0.0f;
        myPid.Reset();
        myLogger.Log(Log::LogLevel::Info, "HeaterController", "Heater disabled");
        return true;
    }

    bool HeaterController::SetTargetTemp(float aTemp)
    {
        myTargetTemp = aTemp;
        myLogger.Log(Log::LogLevel::Debug, "HeaterController", "Target temperature set to {:.1f}", aTemp);
        return true;
    }

    float HeaterController::Update(float aCurrentTemp)
    {
        if (!myIsEnabled)
        {
            myOutput = 0.0f;
            return myOutput;
        }

        myOutput = myPid.Compute(myTargetTemp, aCurrentTemp);
        return myOutput;
    }

    float HeaterController::GetOutput() const
    {
        return myOutput;
    }

    float HeaterController::GetTargetTemp() const
    {
        return myTargetTemp;
    }

    void HeaterController::SetPidTunings(float aKp, float aKi, float aKd)
    {
        myPid.SetTunings(aKp, aKi, aKd);
    }

    void HeaterController::SetPidOutputLimits(float aMin, float aMax)
    {
        myPid.SetOutputLimits(aMin, aMax);
    }
}
