# Web Asset Serving Plan

## Overview

Extend the firmware web stack to serve frontend assets (HTML, JS, CSS, images) so that the frontend can be loaded from the same host/port as the WebSocket server. This allows the frontend's `window.location.host` to correctly resolve to the backend.

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     Browser                                 │
├─────────────────────────────────────────────────────────────┤
│  GET /              → index.html                            │
│  GET /app.js        → JavaScript bundle                     │
│  GET /icons/*.png   → Icon images                           │
│  GET /uPlot.*.js    → Library files                         │
│  WS  /ws            → WebSocket (existing)                  │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                   IHttpServer (new)                         │
│                                                             │
│  Routes HTTP requests to IWebAssetProvider                  │
│  Existing: Routes WebSocket to IMessageHandler              │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│              IWebAssetProvider (new, in lib)                │
│                                                             │
│  + GetAsset(path) → {data, size, mimeType, found}           │
│  + ListAssets() → vector of paths (optional)                │
└─────────────────────────────────────────────────────────────┘
                              │
              ┌───────────────┴───────────────┐
              ▼                               ▼
┌─────────────────────────┐     ┌─────────────────────────────┐
│ FilesystemAssetProvider │     │ SPIFFSAssetProvider         │
│ (simulator)             │     │ (ESP32 - future)            │
│                         │     │                             │
│ Reads from frontend/dist│     │ Reads from SPIFFS partition │
└─────────────────────────┘     └─────────────────────────────┘
```

## Components

### 1. IWebAssetProvider (lib/HeatTreatFurnace/Communication/)

Abstract interface for serving web assets:

```cpp
struct WebAsset
{
    const uint8_t* data;      // Pointer to asset data (may be memory-mapped or buffer)
    size_t size;              // Size in bytes
    etl::string<64> mimeType; // MIME type (text/html, application/javascript, etc.)
    bool found;               // Whether asset exists
};

class IWebAssetProvider
{
public:
    virtual ~IWebAssetProvider() = default;
    
    // Get asset by path (e.g., "/index.html", "/app.js", "/icons/heat.png")
    virtual WebAsset GetAsset(etl::string_view aPath) = 0;
    
    // Release asset data if it was dynamically allocated
    virtual void ReleaseAsset(WebAsset& aAsset) = 0;
};
```

### 2. FilesystemAssetProvider (simulator/src/)

Simulator implementation that reads from filesystem:

```cpp
class FilesystemAssetProvider : public IWebAssetProvider
{
public:
    explicit FilesystemAssetProvider(const std::string& aRootDir);
    
    WebAsset GetAsset(etl::string_view aPath) override;
    void ReleaseAsset(WebAsset& aAsset) override;
    
private:
    std::string myRootDir;
    std::vector<uint8_t> myBuffer; // Reusable buffer for file reads
};
```

Features:
- Serves files from configurable root directory (default: `frontend/dist`)
- Maps `/` to `/index.html`
- Determines MIME type from file extension
- Validates paths to prevent directory traversal attacks

### 3. Update uWebSocketsTransport (simulator/src/)

Extend to handle HTTP requests for static files:

```cpp
class uWebSocketsTransport : public ITransport
{
public:
    // Existing...
    
    // New: Set asset provider for HTTP serving
    void SetAssetProvider(IWebAssetProvider* aProvider);
    
private:
    IWebAssetProvider* myAssetProvider;
};
```

The uWebSockets `App` will be configured with HTTP routes:

```cpp
myApp->get("/*", [this](auto* res, auto* req) {
    std::string path = std::string(req->getUrl());
    if (path == "/") path = "/index.html";
    
    auto asset = myAssetProvider->GetAsset(path);
    if (asset.found) {
        res->writeHeader("Content-Type", asset.mimeType.c_str());
        res->end(std::string_view(reinterpret_cast<const char*>(asset.data), asset.size));
        myAssetProvider->ReleaseAsset(asset);
    } else {
        res->writeStatus("404 Not Found");
        res->end("Not Found");
    }
});
```

### 4. MIME Type Mapping

| Extension | MIME Type |
|-----------|-----------|
| `.html` | `text/html` |
| `.js` | `application/javascript` |
| `.css` | `text/css` |
| `.json` | `application/json` |
| `.png` | `image/png` |
| `.jpg`, `.jpeg` | `image/jpeg` |
| `.svg` | `image/svg+xml` |
| `.ico` | `image/x-icon` |
| `.map` | `application/json` |
| (default) | `application/octet-stream` |

## Frontend Assets Structure

From `frontend/dist/`:
```
dist/
├── index.html          # Main HTML page
├── app.js              # Bundled JavaScript
├── app.js.map          # Source map (optional serving)
├── uPlot.iife.min.js   # Charting library
├── uPlot.min.css       # Chart styles
├── PIDKiln_vars.json   # Config file
└── icons/
    ├── eyec.png
    ├── eyeo.png
    ├── heat.png
    └── menu.png
```

## Security Considerations

1. **Path Traversal Prevention**: Normalize paths and reject any containing `..` or absolute paths
2. **File Size Limits**: Cap maximum file size to prevent memory exhaustion
3. **Allowed Extensions**: Optionally whitelist allowed file extensions

## Tasks

1. Create `IWebAssetProvider` interface in `lib/HeatTreatFurnace/Communication/`
2. Create `FilesystemAssetProvider` in `simulator/src/`
3. Update `uWebSocketsTransport` to handle HTTP GET requests
4. Update `main.cpp` to configure asset provider with frontend/dist path
5. Update README with new command line option for web root directory

## Usage

```bash
# Run simulator serving frontend from default location
./furnace_simulator

# Run with custom frontend location
./furnace_simulator --web-root /path/to/frontend/dist

# Connect browser to http://localhost:3000
```

## Future: ESP32 Implementation

For ESP32, a `SPIFFSAssetProvider` would:
- Read assets from SPIFFS filesystem partition
- Assets uploaded during firmware build/OTA
- Memory-efficient streaming for larger files
