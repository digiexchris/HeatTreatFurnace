#pragma once

#include <catch2/trompeloeil.hpp>
#include "../../lib/Log/LogBackend.hpp"

class MockLogBackend : public LogBackend {
public:
    MAKE_MOCK3(WriteLog, void(LogLevel, std::string_view, std::string_view), override);
    
    MAKE_MOCK2(ShouldLog, bool(LogLevel, std::string_view) const, override);
    
    MAKE_MOCK2(SetLevel, void(std::string_view, LogLevel), override);
    
    MAKE_MOCK1(GetLevel, LogLevel(std::string_view) const, override);
};

