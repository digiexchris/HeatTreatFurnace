#pragma once

#include "Communication/ITransport.hpp"
#include "Communication/IMessageHandler.hpp"
#include "Communication/MessageCodec.hpp"
#include <etl/vector.h>

namespace HeatTreatFurnace::Test
{
    /**
     * @brief Mock transport for unit testing
     *
     * Captures sent messages and allows simulating incoming client messages.
     */
    class MockTransport : public Communication::ITransport
    {
    public:
        struct SentMessage
        {
            uint32_t clientId;
            etl::vector<uint8_t, Communication::MAX_MESSAGE_SIZE> data;
            bool isBroadcast;
        };

        void Send(uint32_t aClientId, const uint8_t* aData, size_t aLen) override
        {
            SentMessage msg;
            msg.clientId = aClientId;
            msg.data.assign(aData, aData + aLen);
            msg.isBroadcast = false;
            mySentMessages.push_back(std::move(msg));
        }

        void Broadcast(const uint8_t* aData, size_t aLen) override
        {
            SentMessage msg;
            msg.clientId = 0;
            msg.data.assign(aData, aData + aLen);
            msg.isBroadcast = true;
            mySentMessages.push_back(std::move(msg));
        }

        void SetMessageHandler(Communication::IMessageHandler* aHandler) override
        {
            myHandler = aHandler;
        }

        /**
         * @brief Simulate an incoming client message
         * @param aClientId Client identifier
         * @param aData Message data
         * @param aLen Message length
         */
        void SimulateClientMessage(uint32_t aClientId, const uint8_t* aData, size_t aLen)
        {
            if (myHandler != nullptr)
            {
                myHandler->HandleMessage(aClientId, aData, aLen);
            }
        }

        /**
         * @brief Get all sent messages
         */
        const etl::vector<SentMessage, 32>& GetSentMessages() const
        {
            return mySentMessages;
        }

        /**
         * @brief Clear sent message history
         */
        void ClearSentMessages()
        {
            mySentMessages.clear();
        }

        /**
         * @brief Get the last sent message (if any)
         */
        const SentMessage* GetLastSentMessage() const
        {
            if (mySentMessages.empty())
            {
                return nullptr;
            }
            return &mySentMessages.back();
        }

        /**
         * @brief Check if a handler is registered
         */
        bool HasHandler() const
        {
            return myHandler != nullptr;
        }

    private:
        Communication::IMessageHandler* myHandler = nullptr;
        etl::vector<SentMessage, 32> mySentMessages;
    };
} // namespace HeatTreatFurnace::Test
