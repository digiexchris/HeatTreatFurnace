#include "LogService.hpp"

#include "ConsoleLogBackend.hpp"

LogService::LogService(std::initializer_list<LogBackendPtr> aBackends)
{
    for (LogBackendPtr backend : aBackends)
    {
        myBackends.push_back(std::move(backend));
    }
}

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
    for (auto it = myBackends.begin(); it != myBackends.end(); ++it)
    {
        if (std::weak_ptr<T> backend = std::dynamic_pointer_cast<T>(*it))
        {
            return backend;
        }
    }

    return nullptr;
}
