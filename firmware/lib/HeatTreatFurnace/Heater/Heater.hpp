//
// Created by chris on 12/14/25.
//

#ifndef TEST_APP_HEATER_HPP
#define TEST_APP_HEATER_HPP

namespace HeatTreatFurnace
{
    namespace Heater
    {
        class Heater
        {
        public:
            bool SetOn();
            bool SetOff();
            bool SetTargetTemp(float temp);
        };
    } // Heater
} // HeatTreatFurnace

#endif //TEST_APP_HEATER_HPP
