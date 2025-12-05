#include "LogBackend.hpp"

void NullLogBackend::WriteLog(LogLevel aLevel, std::string_view aDomain, std::string_view aMessage)
{
}

bool NullLogBackend::ShouldLog(LogLevel aLevel, std::string_view aDomain) const
{
    return false;
}

void NullLogBackend::SetMinLevel(LogLevel aLevel)
{
}

LogLevel NullLogBackend::GetMinLevel() const
{
    return LogLevel::None;
}

