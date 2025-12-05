#pragma once

#include "LogBackend.hpp"
#include <iostream>
#include <map>
#include <string>
#include <set>

class ConsoleLogBackend : public LogBackend
{
public:
    explicit ConsoleLogBackend(bool aUseStderrForErrors = true);

    void WriteLog(LogLevel aLevel, std::string_view aDomain, std::string_view aMessage) override;

    void SetMinLevel(LogLevel aMinLevel) override;

    LogLevel GetMinLevel() const override;

protected:
    // Log if requested level is <= configured level (lower numeric value = higher priority)
    // None=0, Error=1, Warn=2, Info=3, Debug=4, Verbose=5
    bool ShouldLog(LogLevel aLevel, std::string_view aDomain) const override;

private:
    bool myUseStderrForErrors;
    std::set<std::string> myDomains;
    LogLevel myMinLevel;
};

