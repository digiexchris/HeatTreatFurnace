#pragma once

#include "esp_log.h"
#include "../../lib/Log/LogBackend.hpp"

class ESP32LogBackend : public LogBackend {
public:
    void WriteLog(LogLevel aLevel, std::string_view aDomain, std::string_view aMessage) override;
    
    bool ShouldLog(LogLevel aLevel, std::string_view aDomain) const override;
    
    void SetLevel(std::string_view aDomain, LogLevel aLevel) override;
    
    LogLevel GetLevel(std::string_view aDomain) const override;

private:
    esp_log_level_t ConvertLogLevel(LogLevel aLevel) const;
    
    LogLevel ConvertEspLogLevel(esp_log_level_t aEspLevel) const;
};

