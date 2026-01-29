#include "Pid/PidController.hpp"

#include <algorithm>

namespace HeatTreatFurnace::Pid
{
    PidController::PidController(const PidConfig& aConfig)
        : myConfig(aConfig)
    {
    }

    float PidController::Compute(float aSetpoint, float aMeasured)
    {
        float dtSeconds = PrivGetDtSeconds();
        float error = aSetpoint - aMeasured;

        if (!myState.hasPrev)
        {
            myState.prevMeasured = aMeasured;
            myState.prevDerivative = 0.0f;
            myState.hasPrev = true;
        }

        myState.integral += error * dtSeconds;
        myState.integral = PrivClamp(myState.integral, myConfig.integralMin, myConfig.integralMax);

        float rawDerivative = -(aMeasured - myState.prevMeasured) / dtSeconds;
        float filteredDerivative = PrivApplyDerivativeFilter(rawDerivative);

        float output = (myConfig.kp * error) +
                       (myConfig.ki * myState.integral) +
                       (myConfig.kd * filteredDerivative);

        output = PrivClamp(output, myConfig.outputMin, myConfig.outputMax);

        myState.prevMeasured = aMeasured;
        myState.prevDerivative = filteredDerivative;

        return output;
    }

    void PidController::Reset()
    {
        myState = PidState{};
    }

    void PidController::SetTunings(float aKp, float aKi, float aKd)
    {
        myConfig.kp = aKp;
        myConfig.ki = aKi;
        myConfig.kd = aKd;
    }

    void PidController::SetOutputLimits(float aMin, float aMax)
    {
        myConfig.outputMin = aMin;
        myConfig.outputMax = aMax;
    }

    void PidController::SetIntegralLimits(float aMin, float aMax)
    {
        myConfig.integralMin = aMin;
        myConfig.integralMax = aMax;
    }

    float PidController::PrivApplyDerivativeFilter(float aRawDerivative)
    {
        float coeff = PrivClamp(myConfig.derivativeFilterCoeff, 0.0f, 1.0f);
        return (coeff * aRawDerivative) + ((1.0f - coeff) * myState.prevDerivative);
    }

    float PidController::PrivClamp(float aValue, float aMin, float aMax)
    {
        return std::clamp(aValue, aMin, aMax);
    }

    float PidController::PrivGetDtSeconds() const
    {
        float clampedMs = std::max(myConfig.sampleTimeMs, 1.0f);
        return clampedMs / 1000.0f;
    }
}
