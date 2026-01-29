#pragma once

namespace HeatTreatFurnace::Pid
{
    struct PidConfig
    {
        float kp = 2.0f;
        float ki = 0.01f;
        float kd = 50.0f;
        float outputMin = 0.0f;
        float outputMax = 100.0f;
        float integralMin = -1000.0f;
        float integralMax = 1000.0f;
        float derivativeFilterCoeff = 0.2f;
        float sampleTimeMs = 1000.0f;
    };
}
