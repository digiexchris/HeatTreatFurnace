#pragma once

#include "Communication/IWebAssetProvider.hpp"
#include "SimulatorConfig.hpp"
#include <etl/string.h>
#include <etl/vector.h>

namespace Simulator
{
    /**
     * @brief Web asset provider that reads files from the filesystem
     *
     * Used by the simulator to serve frontend assets from a directory
     * (typically frontend/dist).
     */
    class FilesystemAssetProvider : public HeatTreatFurnace::Communication::IWebAssetProvider
    {
    public:
        /**
         * @brief Construct with root directory for assets
         * @param aRootDir Path to directory containing web assets
         */
        explicit FilesystemAssetProvider(const char* aRootDir);

        ~FilesystemAssetProvider() override = default;

        HeatTreatFurnace::Communication::WebAsset GetAsset(etl::string_view aPath) override;
        void ReleaseAsset(HeatTreatFurnace::Communication::WebAsset& aAsset) override;

        /**
         * @brief Check if the root directory exists and is accessible
         */
        [[nodiscard]] bool IsValid() const;

    private:
        bool PrivIsPathSafe(etl::string_view aPath) const;
        void PrivNormalizePath(etl::string_view aPath, etl::string<Config::MAX_PATH_LENGTH>& aOutPath) const;

        etl::string<Config::MAX_PATH_LENGTH> myRootDir;
        etl::vector<uint8_t, Config::MAX_ASSET_SIZE> myBuffer;
    };
} // namespace Simulator
