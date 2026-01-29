#pragma once

#include <cstdint>
#include <cstddef>

namespace HeatTreatFurnace::Communication
{
    class IMessageHandler;

    /**
     * @brief Abstract transport layer for sending/receiving binary messages
     *
     * Implementations:
     * - ESP32WebSocketTransport: Real ESP32 WebSocket handling (future)
     * - MockTransport: Unit test implementation
     * - SimulatorTransport: Frontend testing with local binary (future)
     */
    class ITransport
    {
    public:
        virtual ~ITransport() = default;

        /**
         * @brief Send binary data to a specific client
         * @param aClientId Client identifier (transport-specific)
         * @param aData Pointer to binary data
         * @param aLen Length of data in bytes
         */
        virtual void Send(uint32_t aClientId, const uint8_t* aData, size_t aLen) = 0;

        /**
         * @brief Broadcast binary data to all connected clients
         * @param aData Pointer to binary data
         * @param aLen Length of data in bytes
         */
        virtual void Broadcast(const uint8_t* aData, size_t aLen) = 0;

        /**
         * @brief Set the message handler for incoming messages
         * @param aHandler Pointer to message handler (null to unregister)
         */
        virtual void SetMessageHandler(IMessageHandler* aHandler) = 0;
    };
} // namespace HeatTreatFurnace::Communication
