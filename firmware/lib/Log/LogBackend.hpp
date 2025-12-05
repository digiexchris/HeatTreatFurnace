#pragma once

#include <cstdint>
#include <string_view>
#include "../Enum.hpp"

ENUM(LogLevel, uint16_t,
     None, // No logging
     Error, // Error level
     Warn, // Warning level
     Info, // Information level
     Debug, // Debug level
     Verbose // Verbose level
    );

class LogBackend
{
public:
    virtual ~LogBackend() = default;

    virtual void WriteLog(LogLevel aLevel, std::string_view aDomain, std::string_view aMessage) = 0;


    virtual void SetMinLevel(LogLevel aMinLevel) = 0;

    virtual LogLevel GetMinLevel() const
    {
        return LogLevel::None;
    }

protected:
    // In general, log if requested level is <= configured level (lower numeric value = higher priority)
    // None=0, Error=1, Warn=2, Info=3, Debug=4, Verbose=5
    virtual bool ShouldLog(LogLevel aLevel, std::string_view aDomain) const = 0;
};

class NullLogBackend : public LogBackend
{
public:
    void WriteLog(LogLevel aLevel, std::string_view aDomain, std::string_view aMessage) override;

    void SetMinLevel(LogLevel aMinLevel) override;

    LogLevel GetMinLevel() const override;

protected:
    bool ShouldLog(LogLevel aLevel, std::string_view aDomain) const override;
};

