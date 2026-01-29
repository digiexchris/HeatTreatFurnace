#pragma once

#include "Communication/ITransport.hpp"
#include "Communication/IMessageHandler.hpp"
#include "SimulatorConfig.hpp"
#include <App.h>
#include <unordered_map>
#include <cstdint>
#include <atomic>

namespace Simulator
{
    /**
     * @brief WebSocket transport implementation using uWebSockets
     *
     * Provides WebSocket server functionality for the simulator,
     * managing client connections and routing messages.
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
         * @brief Start the WebSocket server
         * @return true if server started successfully
         */
        bool Start();

        /**
         * @brief Stop the WebSocket server
         */
        void Stop();

        /**
         * @brief Poll for WebSocket events (non-blocking)
         *
         * Call this regularly from the main loop to process WebSocket events.
         */
        void Poll();

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
        std::atomic<bool> myIsRunning;
        uint32_t myNextClientId;

        // Map client ID to WebSocket pointer
        std::unordered_map<uint32_t, WebSocket*> myClients;

        // uWebSockets app and loop
        uWS::App* myApp;
        uWS::Loop* myLoop;
        us_listen_socket_t* myListenSocket;
    };
} // namespace Simulator
