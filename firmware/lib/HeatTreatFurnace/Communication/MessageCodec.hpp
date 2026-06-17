#pragma once

#include "Communication/Generated/furnace_generated.h"
#include "Communication/IStateProvider.hpp"
#include <etl/vector.h>
#include <cstdint>

namespace HeatTreatFurnace::Communication
{
    /**
     * @brief Maximum size for encoded FlatBuffer messages
     */
    constexpr size_t MAX_MESSAGE_SIZE = 4096;

    /**
     * @brief Result of decoding a client message
     */
    struct DecodedClientMessage
    {
        bool isValid = false;
        uint32_t requestId = 0;
        ::Furnace::ClientMessage messageType = ::Furnace::ClientMessage_NONE;
        const ::Furnace::ClientEnvelope* envelope = nullptr;
    };

    /**
     * @brief FlatBuffers message encoding/decoding utilities
     */
    class MessageCodec
    {
    public:
        /**
         * @brief Decode a client envelope from binary data
         * @param aData Pointer to binary data
         * @param aLen Length of data
         * @return DecodedClientMessage with envelope pointer if valid
         */
        static DecodedClientMessage DecodeClientEnvelope(const uint8_t* aData, size_t aLen);

        /**
         * @brief Encode an Ack response
         * @param aRequestId Request ID to echo back
         * @param aSuccess Whether the command succeeded
         * @param aError Error message (optional, null if success)
         * @param aOutBuffer Output buffer for encoded message
         * @return Size of encoded message, 0 on failure
         */
        static size_t EncodeAck(
            uint32_t aRequestId,
            bool aSuccess,
            const char* aError,
            etl::vector<uint8_t, MAX_MESSAGE_SIZE>& aOutBuffer);

        /**
         * @brief Encode an Error response
         * @param aRequestId Request ID to echo back
         * @param aCode Error code
         * @param aMessage Error message
         * @param aOutBuffer Output buffer for encoded message
         * @return Size of encoded message, 0 on failure
         */
        static size_t EncodeError(
            uint32_t aRequestId,
            int32_t aCode,
            const char* aMessage,
            etl::vector<uint8_t, MAX_MESSAGE_SIZE>& aOutBuffer);

        /**
         * @brief Encode a State broadcast message
         * @param aRequestId Request ID (0 for unsolicited broadcast)
         * @param aState Current furnace state data
         * @param aOutBuffer Output buffer for encoded message
         * @return Size of encoded message, 0 on failure
         */
        static size_t EncodeState(
            uint32_t aRequestId,
            const FurnaceStateData& aState,
            etl::vector<uint8_t, MAX_MESSAGE_SIZE>& aOutBuffer);

        /**
         * @brief Encode an empty ProgramListResponse (placeholder)
         * @param aRequestId Request ID to echo back
         * @param aOutBuffer Output buffer for encoded message
         * @return Size of encoded message, 0 on failure
         */
        static size_t EncodeProgramListResponse(
            uint32_t aRequestId,
            etl::vector<uint8_t, MAX_MESSAGE_SIZE>& aOutBuffer);

        /**
         * @brief Encode a PreferencesResponse with placeholder JSON
         * @param aRequestId Request ID to echo back
         * @param aJson JSON string content
         * @param aOutBuffer Output buffer for encoded message
         * @return Size of encoded message, 0 on failure
         */
        static size_t EncodePreferencesResponse(
            uint32_t aRequestId,
            const char* aJson,
            etl::vector<uint8_t, MAX_MESSAGE_SIZE>& aOutBuffer);

        /**
         * @brief Encode a DebugInfoResponse with placeholder JSON
         * @param aRequestId Request ID to echo back
         * @param aJson JSON string content
         * @param aOutBuffer Output buffer for encoded message
         * @return Size of encoded message, 0 on failure
         */
        static size_t EncodeDebugInfoResponse(
            uint32_t aRequestId,
            const char* aJson,
            etl::vector<uint8_t, MAX_MESSAGE_SIZE>& aOutBuffer);
    };
} // namespace HeatTreatFurnace::Communication
