#pragma once

#include <cstdint>
#include <cstddef>

namespace HeatTreatFurnace::Communication
{
    /**
     * @brief Abstract handler for processing incoming binary messages
     *
     * Implementations decode FlatBuffers messages and dispatch to appropriate handlers.
     */
    class IMessageHandler
    {
    public:
        virtual ~IMessageHandler() = default;

        /**
         * @brief Handle an incoming binary message from a client
         * @param aClientId Client identifier (transport-specific)
         * @param aData Pointer to binary message data
         * @param aLen Length of message in bytes
         */
        virtual void HandleMessage(uint32_t aClientId, const uint8_t* aData, size_t aLen) = 0;
    };
} // namespace HeatTreatFurnace::Communication
