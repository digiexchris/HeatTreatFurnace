#ifndef HEAT_TREAT_FURNACE_RESULT_HPP
#define HEAT_TREAT_FURNACE_RESULT_HPP

#include <ostream>
#include <string>
#include "Log/LogService.hpp"

#include <etl/string.h>

namespace HeatTreatFurnace::Furnace
{
    struct Result
    {
        bool success;
        Log::LogMessage message;

        explicit operator bool() const
        {
            return success;
        }

        explicit operator Log::LogMessage() const
        {
            return message;
        }

        [[nodiscard]] bool IsSuccessful() const
        {
            return success;
        }
    };
} // namespace HeatTreatFurnace::Furnace

#endif //HEAT_TREAT_FURNACE_RESULT_HPP
