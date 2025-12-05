#include "ConsoleLogBackend.hpp"

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

bool ConsoleLogBackend::ShouldLog(LogLevel aLevel, std::string_view aDomain) const
{
    if (aLevel == LogLevel::None)
    {
        return false;
    }
    std::string domainStr(aDomain);
    auto it = myDomains.find(domainStr);
    if (it == myDomains.end())
    {
        return false;
    }
    // Log if requested level is <= configured level (lower numeric value = higher priority)
    // None=0, Error=1, Warn=2, Info=3, Debug=4, Verbose=5
    return aLevel <= myMinLevel;
}

void ConsoleLogBackend::SetMinLevel(LogLevel aMinLevel)
{
    myMinLevel = aMinLevel;
}

LogLevel ConsoleLogBackend::GetMinLevel() const
{
    return myMinLevel;
}
