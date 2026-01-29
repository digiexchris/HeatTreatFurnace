#pragma once

namespace HeatTreatFurnace::Heater
{
    class IHeaterController
    {
    public:
        virtual ~IHeaterController() = default;

        virtual bool Enable()
        {
            myIsEnabled = true;
            return true;
        }

        virtual bool Disable()
        {
            myIsEnabled = false;
            return true;
        }

        virtual bool IsEnabled()
        {
            return myIsEnabled;
        }

        virtual bool SetTargetTemp(float aTemp) = 0;

        virtual float Update(float aCurrentTemp) = 0;
        [[nodiscard]] virtual float GetOutput() const = 0;
        [[nodiscard]] virtual float GetTargetTemp() const = 0;

        virtual void SetPidTunings(float aKp, float aKi, float aKd) = 0;
        virtual void SetPidOutputLimits(float aMin, float aMax) = 0;

    protected:
        bool myIsEnabled = false;
    };
}
