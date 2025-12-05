#include "LogBackend.hpp"
#include <string>

void ESP32LogBackend::WriteLog(LogLevel aLevel, std::string_view aDomain, std::string_view aMessage)
{
    std::string domainStr(aDomain);
    const char* tag = domainStr.c_str();
    esp_log_level_t espLevel = ConvertLogLevel(aLevel);
    std::string messageStr(aMessage);
    esp_log_write(espLevel, tag, "%s", messageStr.c_str());
}

bool ESP32LogBackend::ShouldLog(LogLevel aLevel, std::string_view aDomain) const
{
    std::string domainStr(aDomain);
    const char* tag = domainStr.c_str();
    esp_log_level_t currentLevel = esp_log_level_get(tag);
    esp_log_level_t requestedLevel = ConvertLogLevel(aLevel);
    return requestedLevel <= currentLevel;
}

void ESP32LogBackend::SetLevel(std::string_view aDomain, LogLevel aLevel)
{
    std::string domainStr(aDomain);
    const char* tag = domainStr.c_str();
    esp_log_level_t espLevel = ConvertLogLevel(aLevel);
    esp_log_level_set(tag, espLevel);
}

LogLevel ESP32LogBackend::GetLevel(std::string_view aDomain) const
{
    std::string domainStr(aDomain);
    const char* tag = domainStr.c_str();
    esp_log_level_t espLevel = esp_log_level_get(tag);
    return ConvertEspLogLevel(espLevel);
}

esp_log_level_t ESP32LogBackend::ConvertLogLevel(LogLevel aLevel) const
{
    switch (aLevel)
    {
        case LogLevel::None:
            return ESP_LOG_NONE;
        case LogLevel::Error:
            return ESP_LOG_ERROR;
        case LogLevel::Warn:
            return ESP_LOG_WARN;
        case LogLevel::Info:
            return ESP_LOG_INFO;
        case LogLevel::Debug:
            return ESP_LOG_DEBUG;
        case LogLevel::Verbose:
            return ESP_LOG_VERBOSE;
        default:
            return ESP_LOG_NONE;
    }
}

LogLevel ESP32LogBackend::ConvertEspLogLevel(esp_log_level_t aEspLevel) const
{
    switch (aEspLevel)
    {
        case ESP_LOG_NONE:
            return LogLevel::None;
        case ESP_LOG_ERROR:
            return LogLevel::Error;
        case ESP_LOG_WARN:
            return LogLevel::Warn;
        case ESP_LOG_INFO:
            return LogLevel::Info;
        case ESP_LOG_DEBUG:
            return LogLevel::Debug;
        case ESP_LOG_VERBOSE:
            return LogLevel::Verbose;
        default:
            return LogLevel::None;
    }
}

