#pragma once

#include "Communication/Generated/furnace_generated.h"
#include "Communication/MessageCodec.hpp"
#include <etl/vector.h>

namespace HeatTreatFurnace::Test
{
    /**
     * @brief Helper class for encoding client messages in tests
     *
     * Mirrors the frontend encoding pattern for creating test messages.
     */
    class TestClientEncoder
    {
    public:
        static etl::vector<uint8_t, Communication::MAX_MESSAGE_SIZE> EncodeStartCommand(uint32_t aRequestId, int aSegment = 0, int aMinute = 0)
        {
            flatbuffers::FlatBufferBuilder builder(64);

            auto cmd = ::Furnace::CreateStartCommand(builder, aSegment, aMinute);
            auto envelope = ::Furnace::CreateClientEnvelope(
                builder,
                aRequestId,
                ::Furnace::ClientMessage_StartCommand,
                cmd.Union());

            builder.Finish(envelope);
            return ToVector(builder);
        }

        static etl::vector<uint8_t, Communication::MAX_MESSAGE_SIZE> EncodeStopCommand(uint32_t aRequestId)
        {
            flatbuffers::FlatBufferBuilder builder(32);

            auto cmd = ::Furnace::CreateStopCommand(builder);
            auto envelope = ::Furnace::CreateClientEnvelope(
                builder,
                aRequestId,
                ::Furnace::ClientMessage_StopCommand,
                cmd.Union());

            builder.Finish(envelope);
            return ToVector(builder);
        }

        static etl::vector<uint8_t, Communication::MAX_MESSAGE_SIZE> EncodeLoadCommand(uint32_t aRequestId, const char* aProgram)
        {
            flatbuffers::FlatBufferBuilder builder(128);

            auto programOffset = builder.CreateString(aProgram);
            auto cmd = ::Furnace::CreateLoadCommand(builder, programOffset);
            auto envelope = ::Furnace::CreateClientEnvelope(
                builder,
                aRequestId,
                ::Furnace::ClientMessage_LoadCommand,
                cmd.Union());

            builder.Finish(envelope);
            return ToVector(builder);
        }

        static etl::vector<uint8_t, Communication::MAX_MESSAGE_SIZE> EncodeUnloadCommand(uint32_t aRequestId)
        {
            flatbuffers::FlatBufferBuilder builder(32);

            auto cmd = ::Furnace::CreateUnloadCommand(builder);
            auto envelope = ::Furnace::CreateClientEnvelope(
                builder,
                aRequestId,
                ::Furnace::ClientMessage_UnloadCommand,
                cmd.Union());

            builder.Finish(envelope);
            return ToVector(builder);
        }

        static etl::vector<uint8_t, Communication::MAX_MESSAGE_SIZE> EncodeSetTempCommand(uint32_t aRequestId, float aTemperature)
        {
            flatbuffers::FlatBufferBuilder builder(48);

            auto cmd = ::Furnace::CreateSetTempCommand(builder, aTemperature);
            auto envelope = ::Furnace::CreateClientEnvelope(
                builder,
                aRequestId,
                ::Furnace::ClientMessage_SetTempCommand,
                cmd.Union());

            builder.Finish(envelope);
            return ToVector(builder);
        }

        static etl::vector<uint8_t, Communication::MAX_MESSAGE_SIZE> EncodeSetModeCommand(uint32_t aRequestId, ::Furnace::FurnaceMode aMode)
        {
            flatbuffers::FlatBufferBuilder builder(32);

            auto cmd = ::Furnace::CreateSetModeCommand(builder, aMode);
            auto envelope = ::Furnace::CreateClientEnvelope(
                builder,
                aRequestId,
                ::Furnace::ClientMessage_SetModeCommand,
                cmd.Union());

            builder.Finish(envelope);
            return ToVector(builder);
        }

        static etl::vector<uint8_t, Communication::MAX_MESSAGE_SIZE> EncodeListProgramsRequest(uint32_t aRequestId)
        {
            flatbuffers::FlatBufferBuilder builder(32);

            auto req = ::Furnace::CreateListProgramsRequest(builder);
            auto envelope = ::Furnace::CreateClientEnvelope(
                builder,
                aRequestId,
                ::Furnace::ClientMessage_ListProgramsRequest,
                req.Union());

            builder.Finish(envelope);
            return ToVector(builder);
        }

        static etl::vector<uint8_t, Communication::MAX_MESSAGE_SIZE> EncodeGetPreferencesRequest(uint32_t aRequestId)
        {
            flatbuffers::FlatBufferBuilder builder(32);

            auto req = ::Furnace::CreateGetPreferencesRequest(builder);
            auto envelope = ::Furnace::CreateClientEnvelope(
                builder,
                aRequestId,
                ::Furnace::ClientMessage_GetPreferencesRequest,
                req.Union());

            builder.Finish(envelope);
            return ToVector(builder);
        }

        static etl::vector<uint8_t, Communication::MAX_MESSAGE_SIZE> EncodeGetDebugInfoRequest(uint32_t aRequestId)
        {
            flatbuffers::FlatBufferBuilder builder(32);

            auto req = ::Furnace::CreateGetDebugInfoRequest(builder);
            auto envelope = ::Furnace::CreateClientEnvelope(
                builder,
                aRequestId,
                ::Furnace::ClientMessage_GetDebugInfoRequest,
                req.Union());

            builder.Finish(envelope);
            return ToVector(builder);
        }

    private:
        static etl::vector<uint8_t, Communication::MAX_MESSAGE_SIZE> ToVector(flatbuffers::FlatBufferBuilder& aBuilder)
        {
            auto* ptr = aBuilder.GetBufferPointer();
            auto size = aBuilder.GetSize();
            etl::vector<uint8_t, Communication::MAX_MESSAGE_SIZE> result;
            result.assign(ptr, ptr + size);
            return result;
        }
    };
} // namespace HeatTreatFurnace::Test
