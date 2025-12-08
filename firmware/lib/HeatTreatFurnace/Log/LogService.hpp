#pragma once

#include "LogBackend.hpp"
#include <vector>
#include <memory>
#include <format>
#include <string_view>
#include <string>

namespace HeatTreatFurnace::Log
{
    class LogService
    {
    public:
        using LogBackendPtr = std::shared_ptr<LogBackend>;

        explicit LogService(std::vector<LogBackendPtr> aBackends);

        void AddBackend(std::unique_ptr<LogBackend> aBackend);

        template <typename... Args>
        void Log(LogLevel aLevel, std::string_view aDomain, std::string_view aFormat, Args&&... aArgs)
        {
            std::string message = std::vformat(aFormat, std::make_format_args(aArgs...));

            for (auto& backend : myBackends)
            {
                backend->WriteLog(aLevel, aDomain, message);
            }
        }

        //Return a pointer to a particular backend
        //Might be useful for changing a min level after construction of this service
        template <typename T>
        std::weak_ptr<T> GetBackend();

    private:
        std::vector<LogBackendPtr> myBackends;
    };
} //namespace Log