#pragma once

#include "Furnace/Profile/Profile.hpp"
#include <etl/string_view.h>

namespace HeatTreatFurnace::Communication
{
    /**
     * @brief Abstract interface for loading profiles by name
     *
     * Implementations:
     * - Real implementation loads from filesystem/storage (future)
     * - Mock implementation returns pre-configured test profiles
     */
    class IProfileLoader
    {
    public:
        virtual ~IProfileLoader() = default;

        /**
         * @brief Load a profile by name
         * @param aName Profile name to load
         * @param aOutProfile Output profile to populate
         * @return true if profile was found and loaded, false otherwise
         */
        virtual bool LoadProfile(etl::string_view aName, HeatTreatFurnace::Furnace::Profile& aOutProfile) = 0;
    };
} // namespace HeatTreatFurnace::Communication
