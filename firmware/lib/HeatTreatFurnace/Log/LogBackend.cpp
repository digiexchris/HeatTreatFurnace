#include "LogBackend.hpp"

namespace HeatTreatFurnace::Log
{
    void NullLogBackend::WriteLog(LogLevel aLevel, etl::string_view aDomain, etl::string_view aMessage)
    {
    }

    bool NullLogBackend::ShouldLog(LogLevel aLevel) const
    {
        return false;
    }
} //namespace log
