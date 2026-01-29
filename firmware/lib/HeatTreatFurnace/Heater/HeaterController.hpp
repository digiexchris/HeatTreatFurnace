#pragma once

#include "Heater/IHeaterController.hpp"
#include "Log/LogService.hpp"
#include "Pid/PidController.hpp"

namespace HeatTreatFurnace::Heater
{
    class HeaterController : public IHeaterController
    {
    public:
        explicit HeaterController(Log::LogService& aLogger);

        bool Enable() override;
        bool Disable() override;
        bool SetTargetTemp(float aTemp) override;

        float Update(float aCurrentTemp) override;
        [[nodiscard]] float GetOutput() const override;
        [[nodiscard]] float GetTargetTemp() const override;

        void SetPidTunings(float aKp, float aKi, float aKd) override;
        void SetPidOutputLimits(float aMin, float aMax) override;

    private:
        Pid::PidController myPid;
        Log::LogService& myLogger;
        float myTargetTemp = 0.0f;
        float myOutput = 0.0f;
    };
}
