#include "uWebSocketsTransport.hpp"
#include <etl/string.h>
#include <iostream>
#include <cstring>

namespace Simulator
{
    uWebSocketsTransport::uWebSocketsTransport(uint16_t aPort)
        : myPort(aPort)
        , myHandler(nullptr)
        , myAssetProvider(nullptr)
        , myIsRunning(false)
        , myNextClientId(1)
        , myApp(nullptr)
        , myLoop(nullptr)
        , myListenSocket(nullptr)
        , myTickCallback(nullptr)
        , myTickUserData(nullptr)
        , myTickTimer(nullptr)
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

    void uWebSocketsTransport::SetAssetProvider(
        HeatTreatFurnace::Communication::IWebAssetProvider* aProvider)
    {
        myAssetProvider = aProvider;
    }

    bool uWebSocketsTransport::Start()
    {
        if (myIsRunning)
        {
            return true;
        }

        myLoop = uWS::Loop::get();
        myApp = new uWS::App();

        std::cout << "[Simulator] Asset provider: " << (myAssetProvider != nullptr ? "configured" : "not configured") << std::endl;

        // HTTP routes for serving static web assets
        if (myAssetProvider != nullptr)
        {
            std::cout << "[Simulator] Registering HTTP GET /* route" << std::endl;
            myApp->get("/*", [this](auto* res, auto* req) {
                auto urlView = req->getUrl();
                etl::string<Config::MAX_PATH_LENGTH> url(urlView.data(), urlView.size());
                
                std::cout << "[Simulator] HTTP GET " << url.c_str() << std::endl;
                
                // Get asset from provider
                auto asset = myAssetProvider->GetAsset(url.c_str());
                
                if (asset.found)
                {
                    std::cout << "[Simulator] Serving " << url.c_str() << " (" << asset.size << " bytes, " << asset.mimeType.c_str() << ")" << std::endl;
                    res->writeHeader("Content-Type", std::string_view(asset.mimeType.data(), asset.mimeType.size()));
                    res->writeHeader("Cache-Control", "no-cache");
                    res->end(std::string_view(reinterpret_cast<const char*>(asset.data), asset.size));
                    myAssetProvider->ReleaseAsset(asset);
                }
                else
                {
                    std::cout << "[Simulator] 404 Not Found: " << url.c_str() << std::endl;
                    res->writeStatus("404 Not Found");
                    res->writeHeader("Content-Type", "text/plain");
                    res->end("Not Found");
                }
            });
        }

        // WebSocket endpoint
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

    void uWebSocketsTransport::Run()
    {
        if (myApp != nullptr && myIsRunning)
        {
            // Set up tick timer if callback is registered
            if (myTickCallback != nullptr)
            {
                myTickTimer = us_create_timer(reinterpret_cast<us_loop_t*>(myLoop), 0, sizeof(void*));
                *reinterpret_cast<uWebSocketsTransport**>(us_timer_ext(myTickTimer)) = this;

                us_timer_set(myTickTimer, [](us_timer_t* timer) {
                    auto* transport = *reinterpret_cast<uWebSocketsTransport**>(us_timer_ext(timer));
                    if (transport->myTickCallback != nullptr)
                    {
                        transport->myTickCallback(transport->myTickUserData);
                    }
                }, Config::TICK_INTERVAL_MS, Config::TICK_INTERVAL_MS);
            }

            // Run the event loop (blocking)
            myApp->run();
        }
    }

    void uWebSocketsTransport::SetTickCallback(TickCallback aCallback, void* aUserData)
    {
        myTickCallback = aCallback;
        myTickUserData = aUserData;
    }

    void uWebSocketsTransport::RequestStop()
    {
        if (myLoop != nullptr)
        {
            myLoop->defer([this]() {
                Stop();
            });
        }
    }
} // namespace Simulator
