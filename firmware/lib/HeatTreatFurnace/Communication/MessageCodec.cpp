#include "MessageCodec.hpp"
#include <cstring>
#include <etl/vector.h>

namespace HeatTreatFurnace::Communication
{
    DecodedClientMessage MessageCodec::DecodeClientEnvelope(const uint8_t* aData, size_t aLen)
    {
        DecodedClientMessage result;

        if (aData == nullptr || aLen == 0)
        {
            return result;
        }

        flatbuffers::Verifier verifier(aData, aLen);
        if (!verifier.VerifyBuffer<::Furnace::ClientEnvelope>())
        {
            return result;
        }

        result.envelope = flatbuffers::GetRoot<::Furnace::ClientEnvelope>(aData);
        if (result.envelope == nullptr)
        {
            return result;
        }

        result.requestId = result.envelope->request_id();
        result.messageType = result.envelope->message_type();
        result.isValid = true;

        return result;
    }

    size_t MessageCodec::EncodeAck(
        uint32_t aRequestId,
        bool aSuccess,
        const char* aError,
        etl::vector<uint8_t, MAX_MESSAGE_SIZE>& aOutBuffer)
    {
        flatbuffers::FlatBufferBuilder builder(256);

        auto errorOffset = aError ? builder.CreateString(aError) : 0;
        auto ack = ::Furnace::CreateAck(builder, aSuccess, aRequestId, errorOffset);
        auto envelope = ::Furnace::CreateServerEnvelope(
            builder,
            aRequestId,
            ::Furnace::ServerMessage_Ack,
            ack.Union());

        builder.Finish(envelope);

        auto* bufPtr = builder.GetBufferPointer();
        auto bufSize = builder.GetSize();

        aOutBuffer.clear();
        aOutBuffer.resize(bufSize);
        std::memcpy(aOutBuffer.data(), bufPtr, bufSize);

        return bufSize;
    }

    size_t MessageCodec::EncodeError(
        uint32_t aRequestId,
        int32_t aCode,
        const char* aMessage,
        etl::vector<uint8_t, MAX_MESSAGE_SIZE>& aOutBuffer)
    {
        flatbuffers::FlatBufferBuilder builder(256);

        auto messageOffset = aMessage ? builder.CreateString(aMessage) : builder.CreateString("");
        auto error = ::Furnace::CreateError(builder, aCode, messageOffset);
        auto envelope = ::Furnace::CreateServerEnvelope(
            builder,
            aRequestId,
            ::Furnace::ServerMessage_Error,
            error.Union());

        builder.Finish(envelope);

        auto* bufPtr = builder.GetBufferPointer();
        auto bufSize = builder.GetSize();

        aOutBuffer.clear();
        aOutBuffer.resize(bufSize);
        std::memcpy(aOutBuffer.data(), bufPtr, bufSize);

        return bufSize;
    }

    size_t MessageCodec::EncodeState(
        uint32_t aRequestId,
        const FurnaceStateData& aState,
        etl::vector<uint8_t, MAX_MESSAGE_SIZE>& aOutBuffer)
    {
        flatbuffers::FlatBufferBuilder builder(512);

        auto programNameOffset = aState.programName.empty()
                                     ? 0
                                     : builder.CreateString(aState.programName.c_str());
        auto stepOffset = aState.step.empty()
                              ? 0
                              : builder.CreateString(aState.step.c_str());
        auto errorMessageOffset = aState.errorMessage.empty()
                                      ? 0
                                      : builder.CreateString(aState.errorMessage.c_str());

        ::Furnace::StateBuilder stateBuilder(builder);
        stateBuilder.add_mode(aState.mode);
        stateBuilder.add_profile_state(aState.profileState);
        stateBuilder.add_manual_state(aState.manualState);
        if (programNameOffset.o != 0)
        {
            stateBuilder.add_program_name(programNameOffset);
        }
        stateBuilder.add_kiln_temp(aState.kilnTemp);
        stateBuilder.add_set_temp(aState.setTemp);
        stateBuilder.add_env_temp(aState.envTemp);
        stateBuilder.add_case_temp(aState.caseTemp);
        stateBuilder.add_heat_percent(aState.heatPercent);
        stateBuilder.add_temp_change(aState.tempChange);
        if (stepOffset.o != 0)
        {
            stateBuilder.add_step(stepOffset);
        }
        stateBuilder.add_prog_start_ms(aState.progStartMs);
        stateBuilder.add_prog_end_ms(aState.progEndMs);
        stateBuilder.add_curr_time_ms(aState.currTimeMs);
        if (errorMessageOffset.o != 0)
        {
            stateBuilder.add_error_message(errorMessageOffset);
        }
        stateBuilder.add_is_simulator(aState.isSimulator);
        stateBuilder.add_time_scale(aState.timeScale);
        auto state = stateBuilder.Finish();

        auto envelope = ::Furnace::CreateServerEnvelope(
            builder,
            aRequestId,
            ::Furnace::ServerMessage_State,
            state.Union());

        builder.Finish(envelope);

        auto* bufPtr = builder.GetBufferPointer();
        auto bufSize = builder.GetSize();

        aOutBuffer.clear();
        aOutBuffer.resize(bufSize);
        std::memcpy(aOutBuffer.data(), bufPtr, bufSize);

        return bufSize;
    }

    size_t MessageCodec::EncodeProgramListResponse(
        uint32_t aRequestId,
        etl::vector<uint8_t, MAX_MESSAGE_SIZE>& aOutBuffer)
    {
        flatbuffers::FlatBufferBuilder builder(256);

        // Empty program list for now
        etl::vector<flatbuffers::Offset<::Furnace::ProgramInfo>, 8> emptyPrograms;
        auto emptyVector = builder.CreateVector(emptyPrograms.data(), emptyPrograms.size());
        auto response = ::Furnace::CreateProgramListResponse(builder, emptyVector);
        auto envelope = ::Furnace::CreateServerEnvelope(
            builder,
            aRequestId,
            ::Furnace::ServerMessage_ProgramListResponse,
            response.Union());

        builder.Finish(envelope);

        auto* bufPtr = builder.GetBufferPointer();
        auto bufSize = builder.GetSize();

        aOutBuffer.clear();
        aOutBuffer.resize(bufSize);
        std::memcpy(aOutBuffer.data(), bufPtr, bufSize);

        return bufSize;
    }

    size_t MessageCodec::EncodePreferencesResponse(
        uint32_t aRequestId,
        const char* aJson,
        etl::vector<uint8_t, MAX_MESSAGE_SIZE>& aOutBuffer)
    {
        flatbuffers::FlatBufferBuilder builder(256 + std::strlen(aJson));

        auto jsonOffset = builder.CreateString(aJson);
        auto response = ::Furnace::CreatePreferencesResponse(builder, jsonOffset);
        auto envelope = ::Furnace::CreateServerEnvelope(
            builder,
            aRequestId,
            ::Furnace::ServerMessage_PreferencesResponse,
            response.Union());

        builder.Finish(envelope);

        auto* bufPtr = builder.GetBufferPointer();
        auto bufSize = builder.GetSize();

        aOutBuffer.clear();
        aOutBuffer.resize(bufSize);
        std::memcpy(aOutBuffer.data(), bufPtr, bufSize);

        return bufSize;
    }

    size_t MessageCodec::EncodeDebugInfoResponse(
        uint32_t aRequestId,
        const char* aJson,
        etl::vector<uint8_t, MAX_MESSAGE_SIZE>& aOutBuffer)
    {
        flatbuffers::FlatBufferBuilder builder(256 + std::strlen(aJson));

        auto jsonOffset = builder.CreateString(aJson);
        auto response = ::Furnace::CreateDebugInfoResponse(builder, jsonOffset);
        auto envelope = ::Furnace::CreateServerEnvelope(
            builder,
            aRequestId,
            ::Furnace::ServerMessage_DebugInfoResponse,
            response.Union());

        builder.Finish(envelope);

        auto* bufPtr = builder.GetBufferPointer();
        auto bufSize = builder.GetSize();

        aOutBuffer.clear();
        aOutBuffer.resize(bufSize);
        std::memcpy(aOutBuffer.data(), bufPtr, bufSize);

        return bufSize;
    }
} // namespace HeatTreatFurnace::Communication
