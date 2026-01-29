#pragma once

#include "Communication/IProfileLoader.hpp"
#include "SimulatorConfig.hpp"
#include <etl/string.h>
#include <etl/vector.h>

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
        using ProgramName = etl::string<Config::MAX_PROGRAM_NAME_LENGTH>;
        using ProgramList = etl::vector<ProgramName, Config::MAX_PROGRAMS>;

        /**
         * @brief Construct with path to programs directory
         * @param aProgramsDir Path to directory containing program JSON files
         */
        explicit SimulatorProfileLoader(const char* aProgramsDir);

        ~SimulatorProfileLoader() override = default;

        bool LoadProfile(etl::string_view aName,
                         HeatTreatFurnace::Furnace::Profile& aOutProfile) override;

        /**
         * @brief List available program names
         * @param aOutPrograms Vector to populate with program names (without .json extension)
         */
        void ListPrograms(ProgramList& aOutPrograms) const;

    private:
        etl::string<Config::MAX_PATH_LENGTH> myProgramsDir;
    };
} // namespace Simulator
