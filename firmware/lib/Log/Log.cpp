#include <utility>
#include <format>

#include "Log.hpp"

#include <fmt/format.h>

Log::Log(Config aConfig) :
    myConfig(std::move(aConfig))
{
    if (myConfig.console.enable)
    {
        mySinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    }

    if (myConfig.file.enable)
    {
        mySinks.emplace_back(
            std::make_shared<spdlog::sinks::rotating_file_sink_mt>(myConfig.file.path.c_str(), myConfig.file.maxFileSize, myConfig.file.maxFiles));
    }

    if (myConfig.console.enable)
    {
        mySinks.emplace_back(std::make_shared<spdlog::sinks::callback_sink_mt>(myConfig.callback.callback));
    }

    myLogger = std::make_unique<spdlog::logger>("Log", mySinks.begin(), mySinks.end());
}

std::string Log::Format(const std::string aMsg, ...) const
{
    va_list aFormat;
    va_start(aFormat, aMsg);
    return fmt::vformat(aMsg, fmt::make_format_args(aFormat));
    va_end(aFormat);
}


void Log::Debug(const std::string aMsg, ...) const
{
    va_list aFormat;
    va_start(aFormat, aMsg);
    myLogger->debug(Format(aMsg, aFormat));
    va_end(aFormat);
}

void Log::Info(const std::string aMsg, ...) const
{
    va_list aFormat;
    va_start(aFormat, aMsg);
    myLogger->info(aMsg, aFormat);
    va_end(aFormat);
}

void Log::Warn(const std::string aMsg, ...) const
{
    va_list aFormat;
    va_start(aFormat, aMsg);
    myLogger->warn(aMsg);
    va_end(aFormat);
}

void Log::Error(const std::string aMsg, ...) const
{
    va_list aFormat;
    va_start(aFormat, aMsg);
    myLogger->error(aMsg);
    va_end(aFormat);
}
