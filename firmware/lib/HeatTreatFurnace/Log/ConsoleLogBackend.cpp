#include "ConsoleLogBackend.hpp"
#include "LogBackend.hpp"
#include "Furnace/State.hpp"

namespace HeatTreatFurnace::Log
{
    ConsoleLogBackend::ConsoleLogBackend(bool aUseStderrForErrors) :
        myUseStderrForErrors(aUseStderrForErrors),
        myMinLevel(LogLevel::Warn)
    {
    }

    void ConsoleLogBackend::WriteLog(LogLevel aLevel, etl::string_view aDomain, etl::string_view aMessage)
    {
        std::ostream& stream = (myUseStderrForErrors && (aLevel == LogLevel::Error || aLevel == LogLevel::Warn))
            ? std::cerr
            : std::cout;

        stream << "[" << ToString(aLevel) << "] [" << aDomain << "] " << aMessage << "\n";
    }

    bool ConsoleLogBackend::ShouldLog(LogLevel aLevel) const
    {
        bool shouldLog = true;

        if (aLevel == LogLevel::None || aLevel <= myMinLevel)
        {
            shouldLog = false;
        }
        return shouldLog;
    }

    void ConsoleLogBackend::SetMinLevel(LogLevel aMinLevel)
    {
        myMinLevel = aMinLevel;
    }

    LogLevel ConsoleLogBackend::GetMinLevel() const
    {
        return myMinLevel;
    }
} //namespace log
