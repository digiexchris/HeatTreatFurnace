#include "FilesystemAssetProvider.hpp"
#include <fstream>
#include <filesystem>

namespace Simulator
{
    FilesystemAssetProvider::FilesystemAssetProvider(const char* aRootDir)
        : myRootDir(aRootDir)
    {
        // Ensure root directory ends with separator
        if (!myRootDir.empty() && myRootDir.back() != '/')
        {
            myRootDir.append("/");
        }
    }

    HeatTreatFurnace::Communication::WebAsset FilesystemAssetProvider::GetAsset(
        etl::string_view aPath)
    {
        using namespace HeatTreatFurnace::Communication;
        WebAsset asset;
        asset.found = false;

        // Normalize path
        etl::string<Config::MAX_PATH_LENGTH> normalizedPath;
        PrivNormalizePath(aPath, normalizedPath);

        // Check for directory traversal
        if (!PrivIsPathSafe(normalizedPath))
        {
            return asset;
        }

        // Build full filesystem path
        etl::string<Config::MAX_PATH_LENGTH> fullPath = myRootDir;
        fullPath.append(normalizedPath);

        // Check if file exists
        if (!std::filesystem::exists(fullPath.c_str()) ||
            !std::filesystem::is_regular_file(fullPath.c_str()))
        {
            return asset;
        }

        // Get file size
        auto fileSize = std::filesystem::file_size(fullPath.c_str());

        // Limit file size to configured maximum
        if (fileSize > Config::MAX_ASSET_SIZE)
        {
            return asset;
        }

        // Read file into buffer
        std::ifstream file(fullPath.c_str(), std::ios::binary);
        if (!file.is_open())
        {
            return asset;
        }

        myBuffer.resize(static_cast<size_t>(fileSize));
        file.read(reinterpret_cast<char*>(myBuffer.data()), static_cast<std::streamsize>(fileSize));

        if (!file)
        {
            myBuffer.clear();
            return asset;
        }

        // Set asset properties
        asset.data = myBuffer.data();
        asset.size = myBuffer.size();
        asset.mimeType = GetMimeType(normalizedPath);
        asset.found = true;

        return asset;
    }

    void FilesystemAssetProvider::ReleaseAsset(
        HeatTreatFurnace::Communication::WebAsset& aAsset)
    {
        // Buffer is reused, just mark asset as released
        aAsset.data = nullptr;
        aAsset.size = 0;
        aAsset.found = false;
    }

    bool FilesystemAssetProvider::IsValid() const
    {
        return std::filesystem::exists(myRootDir.c_str()) &&
               std::filesystem::is_directory(myRootDir.c_str());
    }

    bool FilesystemAssetProvider::PrivIsPathSafe(etl::string_view aPath) const
    {
        // Reject paths with directory traversal attempts
        if (aPath.find("..") != etl::string_view::npos)
        {
            return false;
        }

        // Reject absolute paths
        if (!aPath.empty() && aPath[0] == '/')
        {
            return false;
        }

        return true;
    }

    void FilesystemAssetProvider::PrivNormalizePath(
        etl::string_view aPath,
        etl::string<Config::MAX_PATH_LENGTH>& aOutPath) const
    {
        aOutPath.clear();

        // Skip leading slash
        size_t start = 0;
        if (!aPath.empty() && aPath[0] == '/')
        {
            start = 1;
        }

        // Map empty path to index.html
        if (start >= aPath.size())
        {
            aOutPath.assign("index.html");
            return;
        }

        aOutPath.assign(aPath.data() + start, aPath.size() - start);
    }
} // namespace Simulator
