#pragma once

#include <doctest/doctest.h>
#include <doctest/trompeloeil.hpp>
#include "etl_profile.h"
#include "Log/LogService.hpp"
#include "../../../lib/HeatTreatFurnace/Furnace/FurnaceFsm.hpp"
#include "mocks/LogBackend.hpp"

namespace HeatTreatFurnace::Test
{
    using namespace HeatTreatFurnace::Furnace;
    using namespace HeatTreatFurnace::Log;
    using trompeloeil::_;

    class FsmTestFixture
    {
    public:
        FsmTestFixture() :
            mockLogBackend(LogLevel::None),
            logger(&mockLogBackend),
            fsm(logger)
        {
        }

        void Init()
        {
            fsm.Init();
        }

        MockLogBackend mockLogBackend;
        LogService logger;
        FurnaceFsm fsm;

        template <typename... Args>
        void SendLog(Log::LogLevel aLevel, BaseState& aState, const etl::string_view& aFormat, Args&&... aArgs)
        {
            logger.Log(aLevel, aState.Name(), aFormat, std::forward<Args>(aArgs)...);
        }
    };
} // namespace HeatTreatFurnace::Test
