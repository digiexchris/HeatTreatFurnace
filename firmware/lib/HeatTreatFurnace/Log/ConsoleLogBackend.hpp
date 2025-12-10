#ifndef CONSOLE_LOG_BACKEND_HPP
#define CONSOLE_LOG_BACKEND_HPP

#include "LogBackend.hpp"
#include <iostream>
#include <map>
#include <string>
#include <set>

namespace HeatTreatFurnace::Log
{
    class ConsoleLogBackend : public LogBackend
    {
    public:
        explicit ConsoleLogBackend(bool aUseStderrForErrors = true);

        void WriteLog(LogLevel aLevel, etl::string_view aDomain, etl::string_view aMessage) override;

        void SetMinLevel(LogLevel aMinLevel) override;

        [[nodiscard]] LogLevel GetMinLevel() const override;

    protected:
        // Log if requested level is <= configured level (lower numeric value = higher priority)
        // None=0, Error=1, Warn=2, Info=3, Debug=4, Verbose=5
        [[nodiscard]] bool ShouldLog(LogLevel aLevel, etl::string_view aDomain) const override;

    private:
        bool myUseStderrForErrors;
        std::set<std::string> myDomains;
        LogLevel myMinLevel;
    };
} //namespace log

#endif // CONSOLE_LOG_BACKEND_HPP
