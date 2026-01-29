#pragma once

#include "Communication/ITransport.hpp"
#include "Communication/IMessageHandler.hpp"
#include "Communication/IWebAssetProvider.hpp"
#include "SimulatorConfig.hpp"
#include <App.h>
#include <etl/unordered_map.h>
#include <cstdint>
#include <atomic>

namespace Simulator
{
    /**
     * @brief WebSocket and HTTP transport implementation using uWebSockets
     *
     * Provides WebSocket server functionality for the simulator,
     * managing client connections and routing messages.
     * Also serves static web assets via HTTP for the frontend.
     */
    class uWebSocketsTransport : public HeatTreatFurnace::Communication::ITransport
    {
    public:
        /**
         * @brief Per-socket user data
         */
        struct PerSocketData
        {
            uint32_t clientId;
        };

        /**
         * @brief Construct transport with specified port
         * @param aPort Port to listen on
         */
        explicit uWebSocketsTransport(uint16_t aPort = Config::DEFAULT_PORT);

        ~uWebSocketsTransport() override;

        // ITransport interface
        void Send(uint32_t aClientId, const uint8_t* aData, size_t aLen) override;
        void Broadcast(const uint8_t* aData, size_t aLen) override;
        void SetMessageHandler(HeatTreatFurnace::Communication::IMessageHandler* aHandler) override;

        /**
         * @brief Set the web asset provider for serving static files
         * @param aProvider Pointer to asset provider (null to disable HTTP serving)
         */
        void SetAssetProvider(HeatTreatFurnace::Communication::IWebAssetProvider* aProvider);

        /**
         * @brief Start the WebSocket server
         * @return true if server started successfully
         */
        bool Start();

        /**
         * @brief Stop the WebSocket server
         */
        void Stop();

        /**
         * @brief Run the event loop (blocking)
         *
         * This runs the uWebSockets event loop. Use SetTickCallback to register
         * periodic work to be done.
         */
        void Run();

        /**
         * @brief Set callback for periodic tick (called every TICK_INTERVAL_MS)
         * @param aCallback Function to call on each tick
         */
        using TickCallback = void(*)(void* aUserData);
        void SetTickCallback(TickCallback aCallback, void* aUserData);

        /**
         * @brief Request the event loop to stop
         */
        void RequestStop();

        /**
         * @brief Check if server is running
         */
        [[nodiscard]] bool IsRunning() const { return myIsRunning; }

        /**
         * @brief Get number of connected clients
         */
        [[nodiscard]] size_t GetClientCount() const { return myClients.size(); }

    private:
        using WebSocket = uWS::WebSocket<false, true, PerSocketData>;

        uint16_t myPort;
        HeatTreatFurnace::Communication::IMessageHandler* myHandler;
        HeatTreatFurnace::Communication::IWebAssetProvider* myAssetProvider;
        std::atomic<bool> myIsRunning;
        uint32_t myNextClientId;

        // Map client ID to WebSocket pointer
        etl::unordered_map<uint32_t, WebSocket*, Config::MAX_CLIENTS> myClients;

        // uWebSockets app and loop
        uWS::App* myApp;
        uWS::Loop* myLoop;
        us_listen_socket_t* myListenSocket;

        // Tick callback
        TickCallback myTickCallback;
        void* myTickUserData;
        struct us_timer_t* myTickTimer;
    };
} // namespace Simulator
