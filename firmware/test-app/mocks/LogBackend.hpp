#pragma once

#include <catch2/trompeloeil.hpp>
#include "Log/LogBackend.hpp"
#include "Log/LogLevel.hpp"
using namespace HeatTreatFurnace::Log;
class MockLogBackend : public HeatTreatFurnace::Log::LogBackend {

public:
    MockLogBackend(LogLevel aMinLogLevel) : LogBackend(aMinLogLevel) {}
    MAKE_MOCK3(WriteLog, void(LogLevel, etl::string_view, etl::string_view), override);

    MAKE_MOCK1(ShouldLog, bool(LogLevel), const override);

    MAKE_MOCK1(SetMinLevel, void(LogLevel), override);

    MAKE_MOCK0(GetMinLevel, LogLevel(), const override);
};

