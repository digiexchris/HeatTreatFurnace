#pragma once

#include <ostream>
#include <string>

namespace HeatTreatFurnace::Furnace
{
    struct Result
    {
        bool success;
        std::string message;

        explicit operator bool() const
        {
            return success;
        }

        explicit operator std::string() const
        {
            return message;
        }

        [[nodiscard]] bool IsSuccessful() const
        {
            return success;
        }
    };
}// namespace HeatTreatFurnace::Furnace