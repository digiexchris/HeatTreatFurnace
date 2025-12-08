#include "LogService.hpp"

#include "ConsoleLogBackend.hpp"

namespace HeatTreatFurnace::Log
{
    LogService::LogService(std::vector<LogBackendPtr> aBackends) :
        myBackends(std::move(aBackends))
    {
    }

    void LogService::AddBackend(std::unique_ptr<LogBackend> aBackend)
    {
        myBackends.push_back(std::move(aBackend));
    }

    template <typename T>
    std::weak_ptr<T> LogService::GetBackend()
    {
        for (auto & myBackend : myBackends)
        {
            if (std::weak_ptr<T> backend = std::dynamic_pointer_cast<T>(myBackend))
            {
                return backend;
            }
        }

        return nullptr;
    }
} //namespace Log