#pragma once

#include "LogLevel.hpp"

namespace HeatTreatFurnace::Log
{
    class LogBackend
    {
    public:
        LogBackend(LogLevel aMinLogLevel) :
            myMinLogLevel(aMinLogLevel)
        {
        }

        virtual ~LogBackend() = default;

        virtual void WriteLog(LogLevel aLevel, etl::string_view aDomain, etl::string_view aMessage) = 0;

        virtual void SetMinLevel(LogLevel aMinLevel)
        {
            myMinLogLevel = aMinLevel;
        }

        [[nodiscard]] virtual LogLevel GetMinLevel() const
        {
            return LogLevel::None;
        }

    protected:
        // In general, log if requested level is <= configured level (lower numeric value = higher priority)
        // None=0, Error=1, Warn=2, Info=3, Debug=4, Verbose=5
        [[nodiscard]] virtual bool ShouldLog(LogLevel aLevel) const
        {
            bool shouldLog = true;

            if (aLevel == LogLevel::None || aLevel <= myMinLogLevel)
            {
                shouldLog = false;
            }
            return shouldLog;
        }

        LogLevel myMinLogLevel;
    };

    class NullLogBackend : public LogBackend
    {
    public:
        NullLogBackend(LogLevel aMinLogLevel = LogLevel::None) :
            LogBackend(aMinLogLevel)
        {
        };
        void WriteLog(LogLevel aLevel, etl::string_view aDomain, etl::string_view aMessage) override;

    protected:
        [[nodiscard]] bool ShouldLog(LogLevel aLevel) const override;
    };
} // namespace log
