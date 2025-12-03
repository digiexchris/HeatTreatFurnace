#include "Log.hpp"

#include <utility>

Log::Log(Config aConfig) : myConfig(std::move(aConfig))
{
    if (myConfig.console.enable)
    {
        mySinks.emplace_back( std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    }

    if (myConfig.file.enable)
    {
        mySinks.emplace_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(myConfig.file.path.c_str(), myConfig.file.maxFileSize, myConfig.file.maxFiles));
    }

    if (myConfig.console.enable)
    {
        mySinks.emplace_back(std::make_shared<spdlog::sinks::callback_sink_mt>(myConfig.callback.callback));
    }

    myLogger = std::make_unique<spdlog::logger>("Log", mySinks.begin(), mySinks.end());
}

void Log::Debug(const std::string& aMsg) const
{
    myLogger->debug(aMsg);
}

void Log::Info(const std::string& aMsg) const
{
    myLogger->info(aMsg);
}

void Log::Warn(const std::string& aMsg) const
{
    myLogger->warn(aMsg);
}

void Log::Error(const std::string& aMsg) const
{
    myLogger->error(aMsg);
}