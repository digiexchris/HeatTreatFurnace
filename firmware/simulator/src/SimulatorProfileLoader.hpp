#pragma once

#include "Communication/IProfileLoader.hpp"
#include "SimulatorConfig.hpp"
#include <string>

namespace Simulator
{
    /**
     * @brief Profile loader that reads JSON program files
     *
     * Loads program definitions from JSON files in the programs directory.
     * JSON format:
     * {
     *   "name": "Test Program",
     *   "description": "A test heat treatment program",
     *   "segments": [
     *     { "target": 100, "ramp_time": 600, "dwell_time": 300 }
     *   ]
     * }
     * Times are in seconds.
     */
    class SimulatorProfileLoader : public HeatTreatFurnace::Communication::IProfileLoader
    {
    public:
        /**
         * @brief Construct with path to programs directory
         * @param aProgramsDir Path to directory containing program JSON files
         */
        explicit SimulatorProfileLoader(const std::string& aProgramsDir);

        ~SimulatorProfileLoader() override = default;

        bool LoadProfile(etl::string_view aName,
                         HeatTreatFurnace::Furnace::Profile& aOutProfile) override;

        /**
         * @brief List available program names
         * @return Vector of program names (without .json extension)
         */
        std::vector<std::string> ListPrograms() const;

    private:
        std::string myProgramsDir;
    };
} // namespace Simulator
