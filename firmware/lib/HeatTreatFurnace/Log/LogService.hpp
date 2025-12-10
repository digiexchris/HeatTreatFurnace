#pragma once

#include "LogBackend.hpp"
#include <format>
#include <string_view>
#include <string>
#include <etl/vector.h>

namespace HeatTreatFurnace::Log
{
    constexpr uint16_t MAX_LOG_BACKENDS = 4;
    static constexpr size_t MAX_MESSAGE_LENGTH = 256;
    using Message = etl::string<MAX_MESSAGE_LENGTH>;

    class LogService
    {
    public:
        // using LogBackendPtr = std::unique_ptr<LogBackend>;
        using LogBackendVec = etl::vector<LogBackend*, MAX_LOG_BACKENDS>;

        template <typename... Args>
        explicit LogService(Args*... aBackends)
        {
            (myBackends.push_back(aBackends), ...);
        }

        void AddBackend(LogBackend* aBackend)
        {
            myBackends.push_back(aBackend);
        }

        template <typename... Args>
        void Log(LogLevel aLevel, const etl::string_view& aDomain, etl::string_view aFormat, Args&&... aArgs)
        {
            Message message = std::format(aFormat, aArgs...);

            for (auto backend : myBackends)
            {
                backend->WriteLog(aLevel, aDomain, message);
            }
        }

    private:
        LogBackendVec myBackends;
    };
} //namespace Log
