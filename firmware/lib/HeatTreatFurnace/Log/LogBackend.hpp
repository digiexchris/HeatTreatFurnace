#pragma once

#include <cstdint>
#include <string_view>

#include "LogLevel.hpp"

namespace HeatTreatFurnace::Log
{
    class LogBackend
    {
    public:
        virtual ~LogBackend() = default;

        virtual void WriteLog(LogLevel aLevel, etl::string_view aDomain, etl::string_view aMessage) = 0;

        virtual void SetMinLevel(LogLevel aMinLevel) = 0;

        [[nodiscard]] virtual LogLevel GetMinLevel() const
        {
            return LogLevel::None;
        }

    protected:
        // In general, log if requested level is <= configured level (lower numeric value = higher priority)
        // None=0, Error=1, Warn=2, Info=3, Debug=4, Verbose=5
        [[nodiscard]] virtual bool ShouldLog(LogLevel aLevel, etl::string_view aDomain) const = 0;
    };

    class NullLogBackend : public LogBackend
    {
    public:
        NullLogBackend() = default;
        void WriteLog(LogLevel aLevel, etl::string_view aDomain, etl::string_view aMessage) override;

        void SetMinLevel(LogLevel aMinLevel) override;

        [[nodiscard]] LogLevel GetMinLevel() const override;

    protected:
        [[nodiscard]] bool ShouldLog(LogLevel aLevel, etl::string_view aDomain) const override;
    };
} // namespace log
