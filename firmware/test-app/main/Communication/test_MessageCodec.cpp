#include <doctest/doctest.h>
#include "Communication/MessageCodec.hpp"
#include "Communication/Generated/furnace_generated.h"
#include "TestClientEncoder.hpp"
#include <cstring>

namespace HeatTreatFurnace::Test
{
    using namespace HeatTreatFurnace::Communication;

    TEST_SUITE("MessageCodec")
    {
        TEST_CASE("DecodeClientEnvelope - valid StartCommand")
        {
            auto msg = TestClientEncoder::EncodeStartCommand(123, 2, 5);

            auto decoded = MessageCodec::DecodeClientEnvelope(msg.data(), msg.size());

            REQUIRE(decoded.isValid);
            REQUIRE(decoded.requestId == 123);
            REQUIRE(decoded.messageType == ::Furnace::ClientMessage_StartCommand);
            REQUIRE(decoded.envelope != nullptr);

            auto* cmd = decoded.envelope->message_as_StartCommand();
            REQUIRE(cmd != nullptr);
            REQUIRE(cmd->segment() == 2);
            REQUIRE(cmd->minute() == 5);
        }

        TEST_CASE("DecodeClientEnvelope - valid LoadCommand")
        {
            auto msg = TestClientEncoder::EncodeLoadCommand(456, "test_profile");

            auto decoded = MessageCodec::DecodeClientEnvelope(msg.data(), msg.size());

            REQUIRE(decoded.isValid);
            REQUIRE(decoded.requestId == 456);
            REQUIRE(decoded.messageType == ::Furnace::ClientMessage_LoadCommand);

            auto* cmd = decoded.envelope->message_as_LoadCommand();
            REQUIRE(cmd != nullptr);
            REQUIRE(cmd->program() != nullptr);
            REQUIRE(std::strcmp(cmd->program()->c_str(), "test_profile") == 0);
        }

        TEST_CASE("DecodeClientEnvelope - valid SetTempCommand")
        {
            auto msg = TestClientEncoder::EncodeSetTempCommand(789, 500.5f);

            auto decoded = MessageCodec::DecodeClientEnvelope(msg.data(), msg.size());

            REQUIRE(decoded.isValid);
            REQUIRE(decoded.requestId == 789);
            REQUIRE(decoded.messageType == ::Furnace::ClientMessage_SetTempCommand);

            auto* cmd = decoded.envelope->message_as_SetTempCommand();
            REQUIRE(cmd != nullptr);
            REQUIRE(cmd->temperature() == doctest::Approx(500.5f));
        }

        TEST_CASE("DecodeClientEnvelope - valid SetModeCommand")
        {
            auto msg = TestClientEncoder::EncodeSetModeCommand(100, ::Furnace::FurnaceMode_Profile);

            auto decoded = MessageCodec::DecodeClientEnvelope(msg.data(), msg.size());

            REQUIRE(decoded.isValid);
            REQUIRE(decoded.requestId == 100);
            REQUIRE(decoded.messageType == ::Furnace::ClientMessage_SetModeCommand);

            auto* cmd = decoded.envelope->message_as_SetModeCommand();
            REQUIRE(cmd != nullptr);
            REQUIRE(cmd->mode() == ::Furnace::FurnaceMode_Profile);
        }

        TEST_CASE("DecodeClientEnvelope - null data returns invalid")
        {
            auto decoded = MessageCodec::DecodeClientEnvelope(nullptr, 0);

            REQUIRE_FALSE(decoded.isValid);
            REQUIRE(decoded.envelope == nullptr);
        }

        TEST_CASE("DecodeClientEnvelope - empty data returns invalid")
        {
            uint8_t data[1] = {0};

            auto decoded = MessageCodec::DecodeClientEnvelope(data, 0);

            REQUIRE_FALSE(decoded.isValid);
        }

        TEST_CASE("DecodeClientEnvelope - malformed data returns invalid")
        {
            uint8_t garbage[] = {0x01, 0x02, 0x03, 0x04, 0x05};

            auto decoded = MessageCodec::DecodeClientEnvelope(garbage, sizeof(garbage));

            REQUIRE_FALSE(decoded.isValid);
        }

        TEST_CASE("EncodeAck - success without error")
        {
            etl::vector<uint8_t, MAX_MESSAGE_SIZE> buffer;

            auto size = MessageCodec::EncodeAck(123, true, nullptr, buffer);

            REQUIRE(size > 0);
            REQUIRE(buffer.size() == size);

            // Verify the encoded message
            flatbuffers::Verifier verifier(buffer.data(), buffer.size());
            REQUIRE(verifier.VerifyBuffer<::Furnace::ServerEnvelope>());

            auto* envelope = ::Furnace::GetServerEnvelope(buffer.data());
            REQUIRE(envelope != nullptr);
            REQUIRE(envelope->request_id() == 123);
            REQUIRE(envelope->message_type() == ::Furnace::ServerMessage_Ack);

            auto* ack = envelope->message_as_Ack();
            REQUIRE(ack != nullptr);
            REQUIRE(ack->success() == true);
            REQUIRE(ack->error() == nullptr);
        }

        TEST_CASE("EncodeAck - failure with error message")
        {
            etl::vector<uint8_t, MAX_MESSAGE_SIZE> buffer;

            auto size = MessageCodec::EncodeAck(456, false, "Something failed", buffer);

            REQUIRE(size > 0);

            auto* envelope = ::Furnace::GetServerEnvelope(buffer.data());
            REQUIRE(envelope != nullptr);
            REQUIRE(envelope->request_id() == 456);

            auto* ack = envelope->message_as_Ack();
            REQUIRE(ack != nullptr);
            REQUIRE(ack->success() == false);
            REQUIRE(ack->error() != nullptr);
            REQUIRE(std::strcmp(ack->error()->c_str(), "Something failed") == 0);
        }

        TEST_CASE("EncodeError - with code and message")
        {
            etl::vector<uint8_t, MAX_MESSAGE_SIZE> buffer;

            auto size = MessageCodec::EncodeError(789, 404, "Not found", buffer);

            REQUIRE(size > 0);

            auto* envelope = ::Furnace::GetServerEnvelope(buffer.data());
            REQUIRE(envelope != nullptr);
            REQUIRE(envelope->request_id() == 789);
            REQUIRE(envelope->message_type() == ::Furnace::ServerMessage_Error);

            auto* error = envelope->message_as_Error();
            REQUIRE(error != nullptr);
            REQUIRE(error->code() == 404);
            REQUIRE(std::strcmp(error->message()->c_str(), "Not found") == 0);
        }

        TEST_CASE("EncodeState - full state data")
        {
            FurnaceStateData state;
            state.mode = ::Furnace::FurnaceMode_Profile;
            state.profileState = ::Furnace::ProfileSubState_Running;
            state.manualState = ::Furnace::ManualSubState_Off;
            state.programName = "TestProgram";
            state.kilnTemp = 750.5f;
            state.setTemp = 800.0f;
            state.envTemp = 25.0f;
            state.caseTemp = 35.0f;
            state.heatPercent = 75;
            state.tempChange = 2.5f;
            state.step = "Ramp to 800";
            state.progStartMs = 1000000;
            state.progEndMs = 2000000;
            state.currTimeMs = 1500000;
            state.isSimulator = false;
            state.timeScale = 1.0f;

            etl::vector<uint8_t, MAX_MESSAGE_SIZE> buffer;
            auto size = MessageCodec::EncodeState(0, state, buffer);

            REQUIRE(size > 0);

            auto* envelope = ::Furnace::GetServerEnvelope(buffer.data());
            REQUIRE(envelope != nullptr);
            REQUIRE(envelope->request_id() == 0);
            REQUIRE(envelope->message_type() == ::Furnace::ServerMessage_State);

            auto* fbState = envelope->message_as_State();
            REQUIRE(fbState != nullptr);
            REQUIRE(fbState->mode() == ::Furnace::FurnaceMode_Profile);
            REQUIRE(fbState->profile_state() == ::Furnace::ProfileSubState_Running);
            REQUIRE(fbState->kiln_temp() == doctest::Approx(750.5f));
            REQUIRE(fbState->set_temp() == doctest::Approx(800.0f));
            REQUIRE(fbState->heat_percent() == 75);
            REQUIRE(std::strcmp(fbState->program_name()->c_str(), "TestProgram") == 0);
            REQUIRE(std::strcmp(fbState->step()->c_str(), "Ramp to 800") == 0);
        }

        TEST_CASE("EncodeProgramListResponse - empty list")
        {
            etl::vector<uint8_t, MAX_MESSAGE_SIZE> buffer;

            auto size = MessageCodec::EncodeProgramListResponse(100, buffer);

            REQUIRE(size > 0);

            auto* envelope = ::Furnace::GetServerEnvelope(buffer.data());
            REQUIRE(envelope != nullptr);
            REQUIRE(envelope->request_id() == 100);
            REQUIRE(envelope->message_type() == ::Furnace::ServerMessage_ProgramListResponse);

            auto* resp = envelope->message_as_ProgramListResponse();
            REQUIRE(resp != nullptr);
            REQUIRE(resp->programs()->size() == 0);
        }

        TEST_CASE("EncodePreferencesResponse - with JSON")
        {
            etl::vector<uint8_t, MAX_MESSAGE_SIZE> buffer;

            auto size = MessageCodec::EncodePreferencesResponse(200, R"({"key": "value"})", buffer);

            REQUIRE(size > 0);

            auto* envelope = ::Furnace::GetServerEnvelope(buffer.data());
            REQUIRE(envelope != nullptr);
            REQUIRE(envelope->request_id() == 200);
            REQUIRE(envelope->message_type() == ::Furnace::ServerMessage_PreferencesResponse);

            auto* resp = envelope->message_as_PreferencesResponse();
            REQUIRE(resp != nullptr);
            REQUIRE(std::strcmp(resp->json()->c_str(), R"({"key": "value"})") == 0);
        }
    }
} // namespace HeatTreatFurnace::Test
