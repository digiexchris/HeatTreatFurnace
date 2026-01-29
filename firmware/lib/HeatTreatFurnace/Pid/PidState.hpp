#pragma once

namespace HeatTreatFurnace::Pid
{
    struct PidState
    {
        float integral = 0.0f;
        float prevMeasured = 0.0f;
        float prevDerivative = 0.0f;
        bool hasPrev = false;
    };
}
