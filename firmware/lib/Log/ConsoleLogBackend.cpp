#include "ConsoleLogBackend.hpp"

#include "Furnace/State.hpp"

namespace log
{
    ConsoleLogBackend::ConsoleLogBackend(bool aUseStderrForErrors) :
        myUseStderrForErrors(aUseStderrForErrors),
        myMinLevel(LogLevel::Warn)
    {
    }

    void ConsoleLogBackend::WriteLog(LogLevel aLevel, std::string_view aDomain, std::string_view aMessage)
    {
        std::ostream& stream = (myUseStderrForErrors && (aLevel == LogLevel::Error || aLevel == LogLevel::Warn))
            ? std::cerr
            : std::cout;

        stream << "[" << ToString(aLevel) << "] [" << aDomain << "] " << aMessage << "\n";
    }

    bool ConsoleLogBackend::ShouldLog(log::LogLevel aLevel, std::string_view aDomain) const
    {
        bool shouldLog = true;

        if (aLevel == log::LogLevel::None)
        {
            shouldLog = false;
        }
        else
        {
            std::string domainStr(aDomain);
            auto it = myDomains.find(domainStr);
            if (it == myDomains.end())
            {
                shouldLog = false;
            }
            else
            {
                // Log if requested level is <= configured level (lower numeric value = higher priority)
                // None=0, Error=1, Warn=2, Info=3, Debug=4, Verbose=5
                shouldLog = aLevel <= myMinLevel;
            }
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
