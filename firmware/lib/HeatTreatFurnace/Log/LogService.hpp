#pragma once

#include "LogBackend.hpp"
#include <format>
#include <string_view>
#include <string>
#include <etl/vector.h>

#include "fmt/base.h"

namespace HeatTreatFurnace::Log
{
    constexpr uint16_t MAX_LOG_BACKENDS = 4;
    static constexpr size_t MAX_MESSAGE_LENGTH = 256;
    using LogMessage = etl::string<MAX_MESSAGE_LENGTH>;
    using LogDomain = etl::string<24>;

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
            LogMessage message(fmt::format(fmt::runtime(aFormat), aArgs...).c_str());

            for (auto backend : myBackends)
            {
                backend->WriteLog(aLevel, aDomain, message);
            }
        }

    private:
        LogBackendVec myBackends;
    };

    class Loggable
    {
    public:
        explicit Loggable(LogService& aLogService) :
            myLogService(aLogService)
        {
        }
        ;
        virtual ~Loggable() = default;

    protected:
        template <typename... Args>
        void Log(LogLevel aLevel, const etl::string_view& aDomain, const etl::string_view& aFormat, Args&&... aArgs)
        {
            myLogService.Log(aLevel, aDomain, aFormat, std::forward<Args>(aArgs)...);
        }

        LogService& myLogService;
    };
} //namespace Log
