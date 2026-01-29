# Furnace Simulator

A C++ simulator for the Heat Treat Furnace that runs on standard Linux x86 systems. It provides a WebSocket server that speaks the same FlatBuffers protocol as the real firmware, allowing frontend development and testing without hardware.

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

The simulator will start a WebSocket server on port 3000 by default.

### Command Line Options

| Option | Description | Default |
|--------|-------------|---------|
| `-p, --port PORT` | WebSocket server port | 3000 |
| `-d, --programs-dir DIR` | Directory containing program JSON files | programs/ |
| `-h, --help` | Show help message | - |

### Example

```bash
# Run on a different port with custom programs directory
./furnace_simulator -p 8080 -d /path/to/programs
```

## Connecting

Connect your frontend to `ws://localhost:3000/ws` (or your configured port). The simulator speaks the same FlatBuffers protocol defined in `proto/furnace.fbs`.

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
│ - Send()        │ │ - State machine │ │ - Heat physics      │
│ - Broadcast()   │ │ - Event queue   │ │ - Time scaling      │
└─────────────────┘ └─────────────────┘ └─────────────────────┘
         │                  ▲
         ▼                  │
┌─────────────────────────────────────────────────────────────┐
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
