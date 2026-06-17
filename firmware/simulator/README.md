# Furnace Simulator

A C++ simulator for the Heat Treat Furnace that runs on standard Linux x86 systems. It provides both an HTTP server for serving the frontend and a WebSocket server that speaks the same FlatBuffers protocol as the real firmware, allowing frontend development and testing without hardware.

## Building

### Prerequisites

- CMake 3.28.3 or later
- C++23 compatible compiler (GCC 13+, Clang 17+)
- OpenSSL development libraries
- zlib

On Debian/Ubuntu:
```bash
sudo apt install build-essential cmake libssl-dev zlib1g-dev
```

### Build Steps

```bash
cd firmware/simulator
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## Running

```bash
./furnace_simulator
```

The simulator will start on port 5173 by default, serving:
- HTTP static files from `frontend/dist/` at `http://localhost:5173/`
- WebSocket endpoint at `ws://localhost:5173/ws`

### Command Line Options

| Option | Description | Default |
|--------|-------------|---------|
| `-p, --port PORT` | Server port | 5173 |
| `-d, --programs-dir DIR` | Directory containing program JSON files | programs/ |
| `-w, --web-root DIR` | Directory containing frontend assets | ../frontend/dist/ |
| `-h, --help` | Show help message | - |

### Example

```bash
# Run on a different port with custom directories
./furnace_simulator -p 8080 -w /path/to/frontend/dist -d /path/to/programs
```

## Connecting

Open `http://localhost:5173` in your browser (or your configured port). The frontend will automatically connect to the WebSocket endpoint at the same host.

## Program Files

Programs are stored as JSON files in the `programs/` directory. Format:

```json
{
  "name": "Program Name",
  "description": "Optional description",
  "segments": [
    {
      "target": 100,
      "ramp_time": 300,
      "dwell_time": 60
    }
  ]
}
```

| Field | Type | Description |
|-------|------|-------------|
| `name` | string | Display name for the program |
| `description` | string | Optional description |
| `segments` | array | List of temperature segments |
| `segments[].target` | number | Target temperature in degrees C |
| `segments[].ramp_time` | number | Time to reach target in seconds |
| `segments[].dwell_time` | number | Time to hold at target in seconds |

## Features

### Frontend Serving

The simulator serves the frontend directly:
- Serves static files from `frontend/dist/` (or custom `--web-root`)
- Handles HTML, JS, CSS, PNG, and other common web assets
- Automatic MIME type detection
- Path traversal protection

### Thermal Simulation

The simulator includes a simple thermal physics model:
- PID-controlled heater output
- Newton's law of cooling
- Configurable thermal mass and heater power
- Derived case temperature

### Time Scaling

Use the `SetTimeScaleCommand` to accelerate simulation time (1x to 100x). This allows testing long programs quickly.

### State Broadcasting

The simulator broadcasts furnace state to all connected clients at regular intervals (default 500ms), including:
- Current temperatures (kiln, case, environment)
- Heater output percentage
- Program progress
- FSM state

## Configuration

Edit `src/SimulatorConfig.hpp` to adjust:
- Thermal parameters (heater power, cooling coefficient, thermal mass)
- PID tuning parameters
- Timing intervals
- Default port and paths

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                        Browser                              │
│                                                             │
│  GET /* ──────► Static files (HTML, JS, CSS, images)        │
│  WS /ws ──────► WebSocket (FlatBuffers protocol)            │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                     Main Event Loop                         │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────┐  │
│  │ Transport   │  │ FSM Queue   │  │ Thermal Simulation  │  │
│  │ Poll()      │  │ Process()   │  │ Update()            │  │
│  └─────────────┘  └─────────────┘  └─────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
         │                  │                    │
         ▼                  ▼                    ▼
┌─────────────────┐ ┌─────────────────┐ ┌─────────────────────┐
│ uWebSockets     │ │ FurnaceFsm      │ │ ThermalSimulator    │
│ Transport       │ │ (from lib)      │ │                     │
│                 │ │                 │ │ - PID Control       │
│ - HTTP GET      │ │ - State machine │ │ - Heat physics      │
│ - WebSocket     │ │ - Event queue   │ │ - Time scaling      │
│ - Send/Broadcast│ │                 │ │                     │
└────────┬────────┘ └─────────────────┘ └─────────────────────┘
         │                  ▲
         │                  │
┌────────▼────────┐         │
│ Filesystem      │         │
│ AssetProvider   │         │
│                 │         │
│ - Reads files   │         │
│ - MIME types    │         │
└─────────────────┘         │
                            │
┌───────────────────────────┴─────────────────────────────────┐
│                  FurnaceMessageHandler                      │
│                     (from lib)                              │
│                                                             │
│  - Decodes FlatBuffers messages                             │
│  - Posts events to FSM                                      │
│  - Sends responses via transport                            │
└─────────────────────────────────────────────────────────────┘
```

## Limitations

- No persistent history storage (history requests return empty)
- No log file storage
- Program save/delete not implemented
- No firmware upload/reboot endpoints
