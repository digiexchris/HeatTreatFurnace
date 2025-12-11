//
// Created by chris on 12/7/25.
//

#ifndef TEST_APP_LOGLEVEL_HPP
#define TEST_APP_LOGLEVEL_HPP
#include <cstdint>

#include "etl/map.h"
#include "etl/string.h"

namespace HeatTreatFurnace::Log
{

    enum class LogLevel: std::uint16_t
    {
        None, // No logging
        Error, // Error level
        Warn, // Warning level
        Info, // Information level
        Debug, // Debug level
        Verbose // Verbose level
    };

    static constexpr size_t MAX_LOG_LEVEL_NAME_LENGTH = 16;
    static constexpr size_t MAX_LOG_LEVELS = 6;

    //MISRA 6.7.2 exception: This is just for lookup of strings for logging purposes.
    inline const etl::map<LogLevel, etl::string<MAX_LOG_LEVEL_NAME_LENGTH>, 6>& logLevelStrings = {
        {LogLevel::None, "None"},
        {LogLevel::Error, "Error"},
        {LogLevel::Warn, "Warn"},
        {LogLevel::Info, "Info"},
        {LogLevel::Debug, "Debug"},
        {LogLevel::Verbose, "Verbose"}
    };

    inline etl::string<MAX_LOG_LEVEL_NAME_LENGTH> ToString(LogLevel aLogLevel)
    {
        return logLevelStrings.at(aLogLevel);
    }

}

#endif //TEST_APP_LOGLEVEL_HPP
