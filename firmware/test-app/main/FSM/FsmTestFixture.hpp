#pragma once

#include <doctest/doctest.h>
#include <doctest/trompeloeil.hpp>
#include "etl_profile.h"
#include "Log/LogService.hpp"
#include "../../../lib/HeatTreatFurnace/Furnace/FurnaceFsm.hpp"
#include "Furnace/FurnaceState.hpp"
#include "mocks/LogBackend.hpp"

namespace HeatTreatFurnace::Test
{
    using namespace HeatTreatFurnace::Furnace;
    using namespace HeatTreatFurnace::Log;
    using namespace HeatTreatFurnace::Furnace;
    using trompeloeil::_;

    class FsmTestFixture
    {
    public:
        FsmTestFixture() :
            mockLogBackend(LogLevel::None),
            logger(&mockLogBackend),
            furnaceState(),
            fsm(furnaceState, logger)
        {
        }

        void Init()
        {
            fsm.Init();
        }

        MockLogBackend mockLogBackend;
        LogService logger;
        FurnaceState furnaceState;
        FurnaceFsm fsm;
    };
} // namespace HeatTreatFurnace::Test
