#pragma once

#include <doctest/doctest.h>
#include <doctest/trompeloeil.hpp>
#include "Heater/IHeaterController.hpp"

class MockHeaterController : public HeatTreatFurnace::Heater::IHeaterController
{
public:
    MockHeaterController() = default;

    // MAKE_MOCK0(Enable, bool(), override);
    // MAKE_MOCK0(Disable, bool(), override);

    MAKE_MOCK1(SetTargetTemp, bool(float aTemp), override);
    // MAKE_CONST_MOCK0(IsEnabled, bool(), override);


    MAKE_MOCK1(Update, float(float aCurrentTemp), override);
    MAKE_CONST_MOCK0(GetOutput, float(), override);
    MAKE_CONST_MOCK0(GetTargetTemp, float(), override);

    MAKE_MOCK3(SetPidTunings, void(float aKp, float aKi, float aKd), override);
    MAKE_MOCK2(SetPidOutputLimits, void(float aMin, float aMax), override);

    bool myIsEnabled = false;
};
