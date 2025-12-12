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
        explicit ConsoleLogBackend(LogLevel aMinLogLevel, bool aUseStderrForErrors = true) :
            LogBackend(aMinLogLevel), myUseStderrForErrors(aUseStderrForErrors)
        {
        }

        void WriteLog(LogLevel aLevel, etl::string_view aDomain, etl::string_view aMessage) override;

    private:
        bool myUseStderrForErrors;
    };
} //namespace log

#endif // CONSOLE_LOG_BACKEND_HPP
