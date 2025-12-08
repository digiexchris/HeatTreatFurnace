#include "LogBackend.hpp"

namespace HeatTreatFurnace::Log
{
    void NullLogBackend::WriteLog(LogLevel aLevel, std::string_view aDomain, std::string_view aMessage)
    {
    }

    bool NullLogBackend::ShouldLog(LogLevel aLevel, std::string_view aDomain) const
    {
        return false;
    }

    void NullLogBackend::SetMinLevel(LogLevel aMinLevel)
    {
    }

    LogLevel NullLogBackend::GetMinLevel() const
    {
        return LogLevel::None;
    }
} //namespace log