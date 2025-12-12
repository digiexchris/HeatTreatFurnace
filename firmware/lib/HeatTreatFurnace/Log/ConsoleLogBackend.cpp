#include "ConsoleLogBackend.hpp"
#include "LogBackend.hpp"
#include "Furnace/State.hpp"

namespace HeatTreatFurnace::Log
{
    void ConsoleLogBackend::WriteLog(LogLevel aLevel, etl::string_view aDomain, etl::string_view aMessage)
    {
        std::ostream& stream = (myUseStderrForErrors && (aLevel == LogLevel::Error || aLevel == LogLevel::Warn))
            ? std::cerr
            : std::cout;

        stream << "[" << ToString(aLevel) << "] [" << aDomain << "] " << aMessage << "\n";
    }
} //namespace log
