# Log Service API Documentation

## Overview

The Log Service provides a flexible, extensible logging system using a service pattern. The `LogService` manages multiple `LogBackend` instances and forwards log messages to all registered backends. This architecture allows for target-agnostic logging with support for multiple output destinations simultaneously.

## Architecture

### Core Components

- **`LogBackend`** - Abstract base class for logging backends
- **`LogService`** - Service class that manages multiple backends and forwards messages
- **Backend Implementations**:
  - `NullLogBackend` - No-op backend (discards all messages)
  - `ConsoleLogBackend` - Outputs to stdout/stderr
  - `ESP32LogBackend` - Integrates with ESP-IDF logging system
  - `MockLogBackend` - Mock backend for testing (trompeloeil)

### Log Levels

```cpp
ENUM(LogLevel, uint8_t,
     None,    // No logging
     Error,   // Error level
     Warn,    // Warning level
     Info,    // Information level
     Debug,   // Debug level
     Verbose  // Verbose level
);
```

## API Reference

### LogBackend

Abstract base class for all logging backends.

#### Methods

```cpp
// Write a log message
virtual void WriteLog(LogLevel aLevel, std::string_view aDomain, std::string_view aMessage) = 0;

// Check if a log level should be logged for a domain
virtual bool ShouldLog(LogLevel aLevel, std::string_view aDomain) const = 0;

// Set log level for a domain
virtual void SetLevel(std::string_view aDomain, LogLevel aLevel);

// Get log level for a domain
virtual LogLevel GetLevel(std::string_view aDomain) const;
```

### LogService

Service class that manages multiple backends and provides a unified logging interface.

#### Constructors

```cpp
// Construct with initializer list of backend pointers (takes ownership)
explicit LogService(std::initializer_list<LogBackend*> aBackends);

// Construct with vector of backend unique_ptrs
explicit LogService(std::vector<std::unique_ptr<LogBackend>> aBackends);
```

#### Methods

```cpp
// Add a backend to the service (takes ownership)
void AddBackend(std::unique_ptr<LogBackend> aBackend);

// Log a formatted message (template method using std::format)
template<typename... Args>
void LogMessage(LogLevel aLevel, std::string_view aDomain, std::string_view aFormat, Args&&... aArgs);

// Set log level for all backends
void SetLevel(std::string_view aDomain, LogLevel aLevel);

// Get log level (returns highest level from all backends)
LogLevel GetLevel(std::string_view aDomain) const;
```

### NullLogBackend

No-op backend that discards all log messages.

#### Usage

```cpp
auto backend = std::make_unique<NullLogBackend>();
auto service = std::make_unique<LogService>(backend.release());
```

### ConsoleLogBackend

Backend that outputs log messages to stdout/stderr.

#### Constructor

```cpp
// aUseStderrForErrors: if true, Error/Warn go to stderr, others to stdout (default: true)
explicit ConsoleLogBackend(bool aUseStderrForErrors = true);
```

#### Output Format

```
[LEVEL] [domain] message
```

#### Usage

```cpp
auto backend = std::make_unique<ConsoleLogBackend>(true);
auto service = std::make_unique<LogService>(backend.release());

service->LogMessage(LogLevel::Error, "app", "Something went wrong: {}", errorCode);
// Outputs to stderr: [Error] [app] Something went wrong: 42
```

### ESP32LogBackend

Backend that integrates with ESP-IDF 5.5 logging system.

#### Usage

```cpp
#include "esp32/main/LogBackend.hpp"

auto backend = std::make_unique<ESP32LogBackend>();
auto service = std::make_unique<LogService>(backend.release());

service->LogMessage(LogLevel::Info, "my_module", "Temperature: {}°C", temp);
// Uses ESP-IDF logging with tag "my_module"
```

### MockLogBackend

Mock backend for unit testing using trompeloeil.

#### Usage

```cpp
#include "mocks/LogBackend.hpp"

auto mockBackend = std::make_unique<MockLogBackend>();
auto service = std::make_unique<LogService>(mockBackend.release());

REQUIRE_CALL(*mockBackend, WriteLog(LogLevel::Info, "test", "message"))
    .TIMES(1);

service->LogMessage(LogLevel::Info, "test", "message");
```

## Usage Examples

### Basic Usage

```cpp
#include "Log/LogService.hpp"
#include "Log/ConsoleLogBackend.hpp"

// Create a service with a console backend
auto service = std::make_unique<LogService>(
    std::make_unique<ConsoleLogBackend>()
);

// Log messages
service->LogMessage(LogLevel::Info, "app", "Application started");
service->LogMessage(LogLevel::Error, "app", "Failed to connect: {}", errorCode);
service->LogMessage(LogLevel::Debug, "network", "Connecting to {}:{}", host, port);
```

### Multiple Backends

```cpp
#include "Log/LogService.hpp"
#include "Log/ConsoleLogBackend.hpp"
#include "esp32/main/LogBackend.hpp"

// Create a service with multiple backends
auto service = std::make_unique<LogService>(
    std::make_unique<ConsoleLogBackend>(),
    std::make_unique<ESP32LogBackend>()
);

// Messages go to both backends
service->LogMessage(LogLevel::Info, "app", "Message logged to both console and ESP-IDF");
```

### Per-Domain Log Levels

```cpp
auto service = std::make_unique<LogService>(
    std::make_unique<ConsoleLogBackend>()
);

// Set different log levels for different domains
service->SetLevel("network", LogLevel::Debug);  // Verbose logging for network
service->SetLevel("app", LogLevel::Info);       // Info level for app
service->SetLevel("sensor", LogLevel::Error);    // Only errors for sensor

// These will be logged
service->LogMessage(LogLevel::Debug, "network", "Packet received");  // Logged
service->LogMessage(LogLevel::Info, "app", "User logged in");        // Logged
service->LogMessage(LogLevel::Error, "sensor", "Read failed");        // Logged

// These will be filtered
service->LogMessage(LogLevel::Verbose, "network", "Debug details");  // Not logged (Debug < Verbose)
service->LogMessage(LogLevel::Debug, "app", "Debug info");          // Not logged (Info < Debug)
service->LogMessage(LogLevel::Warn, "sensor", "Warning");           // Not logged (Error < Warn)
```

### Adding Backends Dynamically

```cpp
auto service = std::make_unique<LogService>(
    std::make_unique<ConsoleLogBackend>()
);

// Add additional backends later
service->AddBackend(std::make_unique<NullLogBackend>());
service->AddBackend(std::make_unique<ESP32LogBackend>());
```

### Integration with StateMachine

```cpp
#include "Furnace/StateMachine.hpp"
#include "Log/LogService.hpp"
#include "Log/ConsoleLogBackend.hpp"

// Create logging service
auto logService = std::make_unique<LogService>(
    std::make_unique<ConsoleLogBackend>()
);

// Pass to StateMachine
StateMachine stateMachine(furnace, logService.get());
```

### Testing with Mock Backend

```cpp
#include <catch2/catch_test_macros.hpp>
#include "Log/LogService.hpp"
#include "mocks/LogBackend.hpp"

TEST_CASE("Logging test")
{
    auto mockBackend = std::make_unique<MockLogBackend>();
    auto service = std::make_unique<LogService>(mockBackend.release());
    
    REQUIRE_CALL(*mockBackend, ShouldLog(LogLevel::Info, "test"))
        .RETURN(true);
    REQUIRE_CALL(*mockBackend, WriteLog(LogLevel::Info, "test", "test message"))
        .TIMES(1);
    
    service->LogMessage(LogLevel::Info, "test", "test message");
}
```

## Design Principles

1. **Target Agnostic**: Base classes contain no platform-specific code
2. **Multiple Backends**: Service can route messages to multiple backends simultaneously
3. **Per-Domain Filtering**: Each backend can filter messages by domain and level
4. **Ownership**: LogService takes ownership of backends via `unique_ptr`
5. **Format Once**: Message formatting happens once in LogService, then forwarded to backends
6. **Type Safety**: Uses C++23 `std::format` for type-safe message formatting

## File Structure

```
firmware/lib/Log/
├── LogBackend.hpp          # Base class and NullLogBackend
├── LogBackend.cpp
├── LogService.hpp          # LogService class
├── LogService.cpp
├── ConsoleLogBackend.hpp   # Console output backend
└── ConsoleLogBackend.cpp

firmware/esp32/main/
├── LogBackend.hpp          # ESP32LogBackend
└── LogBackend.cpp

firmware/test-app/mocks/
└── LogBackend.hpp          # MockLogBackend
```

## Migration Guide

### Before (Old API)

```cpp
Log* myLog = new NullLog();
myLog->LogMessage(LogLevel::Info, "domain", "Message: {}", value);
```

### After (New API)

```cpp
auto service = std::make_unique<LogService>(
    std::make_unique<NullLogBackend>()
);
service->LogMessage(LogLevel::Info, "domain", "Message: {}", value);
```

### Key Changes

1. `Log*` → `LogService*`
2. `Log` → `LogBackend` (base class)
3. `NullLog` → `NullLogBackend`
4. `ESP32Log` → `ESP32LogBackend`
5. `MockLog` → `MockLogBackend`
6. Service takes ownership of backends via `unique_ptr`
7. Multiple backends can be registered with one service

