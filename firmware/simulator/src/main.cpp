#include "SimulatorConfig.hpp"
#include "ThermalSimulator.hpp"
#include "SimulatorStateProvider.hpp"
#include "SimulatorProfileLoader.hpp"
#include "uWebSocketsTransport.hpp"

#include "Furnace/FurnaceFsm.hpp"
#include "Communication/FurnaceMessageHandler.hpp"
#include "Log/LogService.hpp"
#include "Log/ConsoleLogBackend.hpp"

#include <iostream>
#include <chrono>
#include <thread>
#include <csignal>
#include <atomic>

namespace
{
    std::atomic<bool> gRunning{true};

    void SignalHandler(int /*signal*/)
    {
        std::cout << "\n[Simulator] Shutting down..." << std::endl;
        gRunning = false;
    }
}

int main(int argc, char* argv[])
{
    std::cout << "=== Heat Treat Furnace Simulator ===" << std::endl;

    // Parse command line args
    uint16_t port = Simulator::Config::DEFAULT_PORT;
    std::string programsDir = Simulator::Config::PROGRAMS_DIR;

    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];
        if ((arg == "-p" || arg == "--port") && i + 1 < argc)
        {
            port = static_cast<uint16_t>(std::stoi(argv[++i]));
        }
        else if ((arg == "-d" || arg == "--programs-dir") && i + 1 < argc)
        {
            programsDir = argv[++i];
        }
        else if (arg == "-h" || arg == "--help")
        {
            std::cout << "Usage: " << argv[0] << " [options]\n"
                      << "Options:\n"
                      << "  -p, --port PORT        WebSocket port (default: " << Simulator::Config::DEFAULT_PORT << ")\n"
                      << "  -d, --programs-dir DIR Programs directory (default: " << Simulator::Config::PROGRAMS_DIR << ")\n"
                      << "  -h, --help             Show this help\n";
            return 0;
        }
    }

    // Set up signal handler
    std::signal(SIGINT, SignalHandler);
    std::signal(SIGTERM, SignalHandler);

    // Initialize logging
    HeatTreatFurnace::Log::ConsoleLogBackend consoleBackend(HeatTreatFurnace::Log::LogLevel::Debug);
    HeatTreatFurnace::Log::LogService logger(&consoleBackend);

    // Initialize thermal simulator (implements IHeaterController)
    Simulator::ThermalSimulator thermal;

    // Initialize FSM with thermal simulator as heater controller
    HeatTreatFurnace::Furnace::FurnaceFsm fsm(logger, thermal);
    fsm.Init();

    // Initialize state provider
    Simulator::SimulatorStateProvider stateProvider(fsm, thermal);

    // Initialize profile loader
    Simulator::SimulatorProfileLoader profileLoader(programsDir);

    // Initialize WebSocket transport
    Simulator::uWebSocketsTransport transport(port);

    // Initialize message handler
    HeatTreatFurnace::Communication::FurnaceMessageHandler messageHandler(
        fsm, transport, stateProvider, profileLoader, logger);

    // Connect transport to message handler
    transport.SetMessageHandler(&messageHandler);

    // Start WebSocket server
    if (!transport.Start())
    {
        std::cerr << "[Simulator] Failed to start WebSocket server" << std::endl;
        return 1;
    }

    std::cout << "[Simulator] Running on ws://localhost:" << port << Simulator::Config::WS_PATH << std::endl;
    std::cout << "[Simulator] Programs directory: " << programsDir << std::endl;
    std::cout << "[Simulator] Press Ctrl+C to stop" << std::endl;

    // Main loop timing
    auto lastTick = std::chrono::steady_clock::now();
    auto lastBroadcast = lastTick;
    constexpr auto tickInterval = std::chrono::milliseconds(Simulator::Config::TICK_INTERVAL_MS);
    constexpr auto broadcastInterval = std::chrono::milliseconds(Simulator::Config::STATE_BROADCAST_INTERVAL_MS);

    // Main event loop
    while (gRunning)
    {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTick);

        if (elapsed >= tickInterval)
        {
            float deltaSeconds = static_cast<float>(elapsed.count()) / 1000.0f;
            lastTick = now;

            // Process WebSocket events
            transport.Poll();

            // Process FSM event queue
            fsm.ProcessQueue();

            // Update thermal simulation
            thermal.UpdateSimulation(deltaSeconds);

            // Update simulated time
            stateProvider.UpdateTime(static_cast<uint32_t>(elapsed.count()));

            // Broadcast state periodically
            auto sinceLastBroadcast = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastBroadcast);
            if (sinceLastBroadcast >= broadcastInterval && transport.GetClientCount() > 0)
            {
                messageHandler.BroadcastState();
                lastBroadcast = now;
            }
        }
        else
        {
            // Sleep for remaining time
            auto sleepTime = tickInterval - elapsed;
            std::this_thread::sleep_for(sleepTime);
        }
    }

    // Cleanup
    transport.Stop();

    std::cout << "[Simulator] Stopped" << std::endl;
    return 0;
}
