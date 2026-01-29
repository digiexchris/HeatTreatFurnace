#pragma once

#include "Communication/IProfileLoader.hpp"
#include <etl/string.h>
#include <etl/vector.h>

namespace HeatTreatFurnace::Test
{
    /**
     * @brief Mock profile loader that returns pre-configured test profiles
     */
    class MockProfileLoader : public Communication::IProfileLoader
    {
    public:
        bool LoadProfile(etl::string_view aName, HeatTreatFurnace::Furnace::Profile& aOutProfile) override
        {
            etl::string<64> name;
            name.assign(aName.data(), aName.size());

            for (auto& entry : myProfiles)
            {
                if (entry.name == name)
                {
                    aOutProfile = entry.profile;
                    myLastLoadedName = name;
                    return true;
                }
            }
            return false;
        }

        /**
         * @brief Add a profile that can be loaded
         * @param aName Profile name
         * @param aProfile Profile data
         */
        void AddProfile(const char* aName, const Furnace::Profile& aProfile)
        {
            ProfileEntry entry;
            entry.name = aName;
            entry.profile = aProfile;
            myProfiles.push_back(entry);
        }

        /**
         * @brief Remove a profile
         */
        void RemoveProfile(const char* aName)
        {
            for (auto it = myProfiles.begin(); it != myProfiles.end(); ++it)
            {
                if (it->name == aName)
                {
                    myProfiles.erase(it);
                    break;
                }
            }
        }

        /**
         * @brief Clear all profiles
         */
        void Clear()
        {
            myProfiles.clear();
            myLastLoadedName.clear();
        }

        /**
         * @brief Get the last profile name that was requested
         */
        const etl::string<64>& GetLastLoadedName() const
        {
            return myLastLoadedName;
        }

        /**
         * @brief Create a simple test profile with one segment
         */
        static Furnace::Profile CreateSimpleProfile(const char* aName, float aTargetTemp, int aRampMinutes, int aDwellMinutes)
        {
            Furnace::Profile profile;
            profile.name = aName;
            profile.description = "Test profile";

            Furnace::ProfileSegment segment;
            segment.target = aTargetTemp;
            segment.rampTime = std::chrono::minutes(aRampMinutes);
            segment.dwellTime = std::chrono::minutes(aDwellMinutes);
            profile.segments.push_back(segment);

            return profile;
        }

    private:
        struct ProfileEntry
        {
            etl::string<64> name;
            Furnace::Profile profile;
        };

        etl::vector<ProfileEntry, 8> myProfiles;
        etl::string<64> myLastLoadedName;
    };
} // namespace HeatTreatFurnace::Test
