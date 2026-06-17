#include "SimulatorProfileLoader.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>

namespace Simulator
{
    SimulatorProfileLoader::SimulatorProfileLoader(const char* aProgramsDir)
        : myProgramsDir(aProgramsDir)
    {
        // Ensure directory path ends with separator
        if (!myProgramsDir.empty() && myProgramsDir.back() != '/')
        {
            myProgramsDir.append("/");
        }
    }

    bool SimulatorProfileLoader::LoadProfile(
        etl::string_view aName,
        HeatTreatFurnace::Furnace::Profile& aOutProfile)
    {
        // Build file path
        etl::string<Config::MAX_PATH_LENGTH> filePath = myProgramsDir;
        filePath.append(aName.data(), aName.size());
        filePath.append(".json");

        // Try to open file
        std::ifstream file(filePath.c_str());
        if (!file.is_open())
        {
            return false;
        }

        try
        {
            nlohmann::json json;
            file >> json;

            // Parse name (optional, defaults to filename)
            if (json.contains("name"))
            {
                auto name = json["name"].get<std::string>();
                aOutProfile.name = name.c_str();
            }
            else
            {
                aOutProfile.name.assign(aName.data(), aName.size());
            }

            // Parse description (optional)
            if (json.contains("description"))
            {
                auto desc = json["description"].get<std::string>();
                aOutProfile.description = desc.c_str();
            }

            // Parse segments (required)
            if (!json.contains("segments") || !json["segments"].is_array())
            {
                return false;
            }

            aOutProfile.segments.clear();
            for (const auto& seg : json["segments"])
            {
                if (aOutProfile.segments.size() >= HeatTreatFurnace::Furnace::MAX_PROFILE_SEGMENTS)
                {
                    break;
                }

                HeatTreatFurnace::Furnace::ProfileSegment segment;

                // Target temperature (required)
                if (!seg.contains("target"))
                {
                    return false;
                }
                segment.target = seg["target"].get<float>();

                // Ramp time in seconds (required)
                if (!seg.contains("ramp_time"))
                {
                    return false;
                }
                segment.rampTime = std::chrono::seconds(seg["ramp_time"].get<int>());

                // Dwell time in seconds (required)
                if (!seg.contains("dwell_time"))
                {
                    return false;
                }
                segment.dwellTime = std::chrono::seconds(seg["dwell_time"].get<int>());

                aOutProfile.segments.push_back(segment);
            }

            if (aOutProfile.segments.empty())
            {
                return false;
            }

            aOutProfile.isValid = true;
            aOutProfile.currentSegment = 0;
            aOutProfile.currentSegmentTime = std::chrono::seconds(0);
            aOutProfile.runCompleted = false;

            return true;
        }
        catch (const nlohmann::json::exception&)
        {
            return false;
        }
    }

    void SimulatorProfileLoader::ListPrograms(ProgramList& aOutPrograms) const
    {
        aOutPrograms.clear();

        try
        {
            for (const auto& entry : std::filesystem::directory_iterator(myProgramsDir.c_str()))
            {
                if (entry.is_regular_file() && entry.path().extension() == ".json")
                {
                    if (aOutPrograms.size() >= Config::MAX_PROGRAMS)
                    {
                        break;
                    }
                    ProgramName name;
                    name.assign(entry.path().stem().string().c_str());
                    aOutPrograms.push_back(name);
                }
            }
        }
        catch (const std::filesystem::filesystem_error&)
        {
            // Directory doesn't exist or not accessible
        }
    }
} // namespace Simulator
