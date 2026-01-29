#pragma once

#include <cstdint>
#include <cstddef>
#include <etl/string.h>
#include <etl/string_view.h>

namespace HeatTreatFurnace::Communication
{
    /**
     * @brief Represents a web asset (file) to be served
     */
    struct WebAsset
    {
        const uint8_t* data = nullptr;  // Pointer to asset data
        size_t size = 0;                // Size in bytes
        etl::string<64> mimeType;       // MIME type (text/html, application/javascript, etc.)
        bool found = false;             // Whether asset exists
    };

    /**
     * @brief Abstract interface for providing web assets (HTML, JS, CSS, images)
     *
     * Implementations:
     * - FilesystemAssetProvider: Reads from filesystem (simulator)
     * - SPIFFSAssetProvider: Reads from SPIFFS partition (ESP32, future)
     */
    class IWebAssetProvider
    {
    public:
        virtual ~IWebAssetProvider() = default;

        /**
         * @brief Get asset by URL path
         * @param aPath URL path (e.g., "/index.html", "/app.js", "/icons/heat.png")
         * @return WebAsset with data pointer, size, and MIME type. Check 'found' field.
         *
         * The path "/" should typically map to "/index.html".
         * Implementations should prevent directory traversal attacks.
         */
        virtual WebAsset GetAsset(etl::string_view aPath) = 0;

        /**
         * @brief Release asset data if it was dynamically allocated
         * @param aAsset Asset to release
         *
         * Call this after sending the asset to free any allocated memory.
         * Safe to call on assets that don't need releasing.
         */
        virtual void ReleaseAsset(WebAsset& aAsset) = 0;
    };

    /**
     * @brief Get MIME type for a file extension
     * @param aPath File path or name
     * @return MIME type string
     */
    inline etl::string<64> GetMimeType(etl::string_view aPath)
    {
        // Find last dot
        size_t dotPos = aPath.size();
        for (size_t i = aPath.size(); i > 0; --i)
        {
            if (aPath[i - 1] == '.')
            {
                dotPos = i - 1;
                break;
            }
            if (aPath[i - 1] == '/')
            {
                break;
            }
        }

        if (dotPos >= aPath.size())
        {
            return "application/octet-stream";
        }

        etl::string_view ext = aPath.substr(dotPos);

        if (ext == ".html" || ext == ".htm")
            return "text/html; charset=utf-8";
        if (ext == ".js")
            return "application/javascript; charset=utf-8";
        if (ext == ".css")
            return "text/css; charset=utf-8";
        if (ext == ".json")
            return "application/json; charset=utf-8";
        if (ext == ".png")
            return "image/png";
        if (ext == ".jpg" || ext == ".jpeg")
            return "image/jpeg";
        if (ext == ".gif")
            return "image/gif";
        if (ext == ".svg")
            return "image/svg+xml";
        if (ext == ".ico")
            return "image/x-icon";
        if (ext == ".map")
            return "application/json";
        if (ext == ".woff")
            return "font/woff";
        if (ext == ".woff2")
            return "font/woff2";
        if (ext == ".ttf")
            return "font/ttf";

        return "application/octet-stream";
    }
} // namespace HeatTreatFurnace::Communication
