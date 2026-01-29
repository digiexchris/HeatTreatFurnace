#include "SimulatorConfig.hpp"
#include "ThermalSimulator.hpp"
#include "SimulatorStateProvider.hpp"
#include "SimulatorProfileLoader.hpp"
#include "FilesystemAssetProvider.hpp"
#include "uWebSocketsTransport.hpp"

#include "Furnace/FurnaceFsm.hpp"
#include "Communication/FurnaceMessageHandler.hpp"
#include "Log/LogService.hpp"
#include "Log/ConsoleLogBackend.hpp"

#include <etl/string.h>
#include <iostream>
#include <chrono>
#include <csignal>
#include <cstring>
#include <cstdlib>

namespace
{
    Simulator::uWebSocketsTransport* gTransport = nullptr;

    void SignalHandler(int /*signal*/)
    {
        std::cout << "\n[Simulator] Shutting down..." << std::endl;
        if (gTransport != nullptr)
        {
            gTransport->RequestStop();
        }
    }

    struct TickContext
    {
        HeatTreatFurnace::Furnace::FurnaceFsm* fsm;
        Simulator::ThermalSimulator* thermal;
        Simulator::SimulatorStateProvider* stateProvider;
        HeatTreatFurnace::Communication::FurnaceMessageHandler* messageHandler;
        Simulator::uWebSocketsTransport* transport;
        std::chrono::steady_clock::time_point lastBroadcast;
    };

    void OnTick(void* aUserData)
    {
        auto* ctx = static_cast<TickContext*>(aUserData);

        // Process FSM event queue
        ctx->fsm->ProcessQueue();

        // Update thermal simulation
        float deltaSeconds = static_cast<float>(Simulator::Config::TICK_INTERVAL_MS) / 1000.0f;
        ctx->thermal->UpdateSimulation(deltaSeconds);

        // Update simulated time
        ctx->stateProvider->UpdateTime(Simulator::Config::TICK_INTERVAL_MS);

        // Broadcast state periodically
        auto now = std::chrono::steady_clock::now();
        auto sinceLastBroadcast = std::chrono::duration_cast<std::chrono::milliseconds>(now - ctx->lastBroadcast);
        if (sinceLastBroadcast.count() >= Simulator::Config::STATE_BROADCAST_INTERVAL_MS &&
            ctx->transport->GetClientCount() > 0)
        {
            ctx->messageHandler->BroadcastState();
            ctx->lastBroadcast = now;
        }
    }
}

int main(int argc, char* argv[])
{
    std::cout << "=== Heat Treat Furnace Simulator ===" << std::endl;

    // Parse command line args
    uint16_t port = Simulator::Config::DEFAULT_PORT;
    etl::string<Simulator::Config::MAX_PATH_LENGTH> programsDir(Simulator::Config::PROGRAMS_DIR);
    etl::string<Simulator::Config::MAX_PATH_LENGTH> webRoot(Simulator::Config::DEFAULT_WEB_ROOT);

    for (int i = 1; i < argc; i++)
    {
        if ((std::strcmp(argv[i], "-p") == 0 || std::strcmp(argv[i], "--port") == 0) && i + 1 < argc)
        {
            port = static_cast<uint16_t>(std::atoi(argv[++i]));
        }
        else if ((std::strcmp(argv[i], "-d") == 0 || std::strcmp(argv[i], "--programs-dir") == 0) && i + 1 < argc)
        {
            programsDir = argv[++i];
        }
        else if ((std::strcmp(argv[i], "-w") == 0 || std::strcmp(argv[i], "--web-root") == 0) && i + 1 < argc)
        {
            webRoot = argv[++i];
        }
        else if (std::strcmp(argv[i], "-h") == 0 || std::strcmp(argv[i], "--help") == 0)
        {
            std::cout << "Usage: " << argv[0] << " [options]\n"
                      << "Options:\n"
                      << "  -p, --port PORT        Server port (default: " << Simulator::Config::DEFAULT_PORT << ")\n"
                      << "  -d, --programs-dir DIR Programs directory (default: " << Simulator::Config::PROGRAMS_DIR << ")\n"
                      << "  -w, --web-root DIR     Web assets directory (default: " << Simulator::Config::DEFAULT_WEB_ROOT << ")\n"
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
    Simulator::SimulatorProfileLoader profileLoader(programsDir.c_str());

    // Initialize web asset provider
    Simulator::FilesystemAssetProvider assetProvider(webRoot.c_str());
    if (!assetProvider.IsValid())
    {
        std::cerr << "[Simulator] Warning: Web root directory not found: " << webRoot.c_str() << std::endl;
        std::cerr << "[Simulator] HTTP asset serving will be disabled" << std::endl;
    }

    // Initialize WebSocket transport
    Simulator::uWebSocketsTransport transport(port);
    gTransport = &transport;

    // Configure asset provider for HTTP serving
    if (assetProvider.IsValid())
    {
        transport.SetAssetProvider(&assetProvider);
    }

    // Initialize message handler
    HeatTreatFurnace::Communication::FurnaceMessageHandler messageHandler(
        fsm, transport, stateProvider, profileLoader, logger);

    // Connect transport to message handler
    transport.SetMessageHandler(&messageHandler);

    // Set up tick context for periodic updates
    TickContext tickCtx{
        &fsm,
        &thermal,
        &stateProvider,
        &messageHandler,
        &transport,
        std::chrono::steady_clock::now()
    };
    transport.SetTickCallback(OnTick, &tickCtx);

    // Start server
    if (!transport.Start())
    {
        std::cerr << "[Simulator] Failed to start server" << std::endl;
        return 1;
    }

    std::cout << "[Simulator] Running on http://localhost:" << port << std::endl;
    std::cout << "[Simulator] WebSocket endpoint: ws://localhost:" << port << Simulator::Config::WS_PATH << std::endl;
    std::cout << "[Simulator] Programs directory: " << programsDir.c_str() << std::endl;
    if (assetProvider.IsValid())
    {
        std::cout << "[Simulator] Web root: " << webRoot.c_str() << std::endl;
    }
    std::cout << "[Simulator] Press Ctrl+C to stop" << std::endl;

    // Run the event loop (blocks until RequestStop is called)
    transport.Run();

    // Cleanup
    gTransport = nullptr;

    std::cout << "[Simulator] Stopped" << std::endl;
    return 0;
}
