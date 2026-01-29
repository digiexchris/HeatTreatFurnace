#pragma once

#include "PidConfig.hpp"
#include "PidState.hpp"

namespace HeatTreatFurnace::Pid
{
    class PidController
    {
    public:
        explicit PidController(const PidConfig& aConfig);

        float Compute(float aSetpoint, float aMeasured);
        void Reset();
        void SetTunings(float aKp, float aKi, float aKd);
        void SetOutputLimits(float aMin, float aMax);
        void SetIntegralLimits(float aMin, float aMax);

    private:
        float PrivApplyDerivativeFilter(float aRawDerivative);
        float PrivClamp(float aValue, float aMin, float aMax);
        [[nodiscard]] float PrivGetDtSeconds() const;

        PidConfig myConfig;
        PidState myState;
    };
}
