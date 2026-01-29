#include "SimulatorProfileLoader.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>

namespace Simulator
{
    SimulatorProfileLoader::SimulatorProfileLoader(const std::string& aProgramsDir)
        : myProgramsDir(aProgramsDir)
    {
        // Ensure directory path ends with separator
        if (!myProgramsDir.empty() && myProgramsDir.back() != '/')
        {
            myProgramsDir += '/';
        }
    }

    bool SimulatorProfileLoader::LoadProfile(
        etl::string_view aName,
        HeatTreatFurnace::Furnace::Profile& aOutProfile)
    {
        // Build file path
        std::string filePath = myProgramsDir + std::string(aName.data(), aName.size()) + ".json";

        // Try to open file
        std::ifstream file(filePath);
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
                std::string name = json["name"].get<std::string>();
                aOutProfile.name = name.c_str();
            }
            else
            {
                aOutProfile.name.assign(aName.data(), aName.size());
            }

            // Parse description (optional)
            if (json.contains("description"))
            {
                std::string desc = json["description"].get<std::string>();
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

    std::vector<std::string> SimulatorProfileLoader::ListPrograms() const
    {
        std::vector<std::string> programs;

        try
        {
            for (const auto& entry : std::filesystem::directory_iterator(myProgramsDir))
            {
                if (entry.is_regular_file() && entry.path().extension() == ".json")
                {
                    programs.push_back(entry.path().stem().string());
                }
            }
        }
        catch (const std::filesystem::filesystem_error&)
        {
            // Directory doesn't exist or not accessible
        }

        return programs;
    }
} // namespace Simulator
