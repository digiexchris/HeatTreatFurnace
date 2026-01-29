#pragma once

#include "Heater/IHeaterController.hpp"
#include "Pid/PidController.hpp"
#include "Pid/PidConfig.hpp"
#include "SimulatorConfig.hpp"

namespace Simulator
{
    /**
     * @brief Simulated heater controller with thermal physics
     *
     * Implements IHeaterController to provide temperature simulation for the FSM.
     * Uses PID control for heater output and simple thermal physics model.
     */
    class ThermalSimulator : public HeatTreatFurnace::Heater::IHeaterController
    {
    public:
        ThermalSimulator();
        ~ThermalSimulator() override = default;

        // IHeaterController interface
        bool SetTargetTemp(float aTemp) override;
        float Update(float aCurrentTemp) override;
        [[nodiscard]] float GetOutput() const override;
        [[nodiscard]] float GetTargetTemp() const override;
        void SetPidTunings(float aKp, float aKi, float aKd) override;
        void SetPidOutputLimits(float aMin, float aMax) override;

        /**
         * @brief Advance thermal simulation by delta time
         * @param aDeltaSeconds Real-time seconds elapsed (will be scaled by time scale)
         */
        void UpdateSimulation(float aDeltaSeconds);

        /**
         * @brief Get current simulated kiln temperature
         */
        [[nodiscard]] float GetKilnTemp() const { return myKilnTemp; }

        /**
         * @brief Get current simulated case temperature
         */
        [[nodiscard]] float GetCaseTemp() const { return myCaseTemp; }

        /**
         * @brief Get current simulated environmental temperature
         */
        [[nodiscard]] float GetEnvTemp() const { return Config::AMBIENT_TEMP; }

        /**
         * @brief Get current heat output percentage (0-100)
         */
        [[nodiscard]] uint8_t GetHeatPercent() const { return static_cast<uint8_t>(myHeatOutput); }

        /**
         * @brief Get temperature change rate (degC/sec)
         */
        [[nodiscard]] float GetTempChangeRate() const { return myTempChangeRate; }

        /**
         * @brief Set time scale for simulation
         * @param aScale Time scale factor (1.0 = real-time, 10.0 = 10x faster)
         */
        void SetTimeScale(float aScale);

        /**
         * @brief Get current time scale
         */
        [[nodiscard]] float GetTimeScale() const { return myTimeScale; }

    private:
        HeatTreatFurnace::Pid::PidController myPid;
        float myTargetTemp;
        float myKilnTemp;
        float myCaseTemp;
        float myHeatOutput;
        float myTempChangeRate;
        float myTimeScale;
    };
} // namespace Simulator
