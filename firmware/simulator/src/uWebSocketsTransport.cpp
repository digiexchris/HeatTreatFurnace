#include "uWebSocketsTransport.hpp"
#include <iostream>
#include <cstring>

namespace Simulator
{
    uWebSocketsTransport::uWebSocketsTransport(uint16_t aPort)
        : myPort(aPort)
        , myHandler(nullptr)
        , myIsRunning(false)
        , myNextClientId(1)
        , myApp(nullptr)
        , myLoop(nullptr)
        , myListenSocket(nullptr)
    {
    }

    uWebSocketsTransport::~uWebSocketsTransport()
    {
        Stop();
    }

    void uWebSocketsTransport::Send(uint32_t aClientId, const uint8_t* aData, size_t aLen)
    {
        auto it = myClients.find(aClientId);
        if (it != myClients.end() && it->second != nullptr)
        {
            std::string_view message(reinterpret_cast<const char*>(aData), aLen);
            it->second->send(message, uWS::OpCode::BINARY);
        }
    }

    void uWebSocketsTransport::Broadcast(const uint8_t* aData, size_t aLen)
    {
        std::string_view message(reinterpret_cast<const char*>(aData), aLen);
        for (auto& [clientId, ws] : myClients)
        {
            if (ws != nullptr)
            {
                ws->send(message, uWS::OpCode::BINARY);
            }
        }
    }

    void uWebSocketsTransport::SetMessageHandler(
        HeatTreatFurnace::Communication::IMessageHandler* aHandler)
    {
        myHandler = aHandler;
    }

    bool uWebSocketsTransport::Start()
    {
        if (myIsRunning)
        {
            return true;
        }

        myLoop = uWS::Loop::get();
        myApp = new uWS::App();

        myApp->ws<PerSocketData>(Config::WS_PATH, {
            .compression = uWS::DISABLED,
            .maxPayloadLength = 16 * 1024,
            .idleTimeout = 120,
            .maxBackpressure = 1 * 1024 * 1024,

            .open = [this](WebSocket* ws) {
                auto* userData = ws->getUserData();
                userData->clientId = myNextClientId++;
                myClients[userData->clientId] = ws;
                std::cout << "[Simulator] Client " << userData->clientId << " connected" << std::endl;
            },

            .message = [this](WebSocket* ws, std::string_view message, uWS::OpCode opCode) {
                if (opCode == uWS::OpCode::BINARY && myHandler != nullptr)
                {
                    auto* userData = ws->getUserData();
                    myHandler->HandleMessage(
                        userData->clientId,
                        reinterpret_cast<const uint8_t*>(message.data()),
                        message.size());
                }
            },

            .close = [this](WebSocket* ws, int /*code*/, std::string_view /*message*/) {
                auto* userData = ws->getUserData();
                std::cout << "[Simulator] Client " << userData->clientId << " disconnected" << std::endl;
                myClients.erase(userData->clientId);
            }
        });

        myApp->listen(myPort, [this](us_listen_socket_t* listenSocket) {
            myListenSocket = listenSocket;
            if (listenSocket)
            {
                std::cout << "[Simulator] WebSocket server listening on port " << myPort << std::endl;
                myIsRunning = true;
            }
            else
            {
                std::cerr << "[Simulator] Failed to listen on port " << myPort << std::endl;
            }
        });

        return myIsRunning;
    }

    void uWebSocketsTransport::Stop()
    {
        if (!myIsRunning)
        {
            return;
        }

        myIsRunning = false;

        // Close all client connections
        for (auto& [clientId, ws] : myClients)
        {
            if (ws != nullptr)
            {
                ws->close();
            }
        }
        myClients.clear();

        // Close listen socket
        if (myListenSocket != nullptr)
        {
            us_listen_socket_close(0, myListenSocket);
            myListenSocket = nullptr;
        }

        // Clean up app
        if (myApp != nullptr)
        {
            delete myApp;
            myApp = nullptr;
        }

        std::cout << "[Simulator] WebSocket server stopped" << std::endl;
    }

    void uWebSocketsTransport::Poll()
    {
        if (myLoop != nullptr && myIsRunning)
        {
            // Process pending events without blocking
            myLoop->integrate();
        }
    }
} // namespace Simulator
