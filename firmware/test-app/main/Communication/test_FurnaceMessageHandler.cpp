#include <doctest/doctest.h>
#include <doctest/trompeloeil.hpp>
#include "Communication/FurnaceMessageHandler.hpp"
#include "Communication/Generated/furnace_generated.h"
#include "mocks/MockTransport.hpp"
#include "mocks/MockStateProvider.hpp"
#include "mocks/MockProfileLoader.hpp"
#include "TestClientEncoder.hpp"
#include "mocks/LogBackend.hpp"
#include "mocks/HeaterController.hpp"
#include <cstring>

namespace HeatTreatFurnace::Test
{
    using namespace HeatTreatFurnace::Communication;
    using namespace HeatTreatFurnace::Furnace;
    using trompeloeil::_;

    class MessageHandlerTestFixture
    {
    public:
        MessageHandlerTestFixture()
            : mockLogBackend(Log::LogLevel::None)
              , logger(&mockLogBackend)
              , fsm(logger, mockHeater)
        {
            mockStateProvider.Reset();
        }

        void Init()
        {
            fsm.Init();
            handler = std::make_unique<FurnaceMessageHandler>(
                fsm, mockTransport, mockStateProvider, mockProfileLoader, logger);
            mockTransport.SetMessageHandler(handler.get());
        }

        MockLogBackend mockLogBackend;
        Log::LogService logger;
        MockHeaterController mockHeater;
        FurnaceFsm fsm;
        MockTransport mockTransport;
        MockStateProvider mockStateProvider;
        MockProfileLoader mockProfileLoader;
        std::unique_ptr<FurnaceMessageHandler> handler;
    };

    TEST_SUITE("FurnaceMessageHandler")
    {
        TEST_CASE("StartCommand posts EvtProfileStart to FSM")
        {
            MessageHandlerTestFixture fixture;
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_, _, _));
            fixture.Init();

            // Put FSM in profile mode with loaded program first
            fixture.fsm.Post(EvtModeProfile());
            Furnace::Profile profile;
            profile.name = "TestProfile";
            profile.segments.push_back({100.0f, std::chrono::minutes(10), std::chrono::minutes(5)});
            fixture.fsm.Post(EvtProfileLoad(profile));
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_LOADED);

            // Send StartCommand
            auto msg = TestClientEncoder::EncodeStartCommand(1);
            fixture.mockTransport.SimulateClientMessage(100, msg.data(), msg.size());
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_RUNNING);

            // Verify Ack was sent
            auto* sent = fixture.mockTransport.GetLastSentMessage();
            REQUIRE(sent != nullptr);
            auto* envelope = ::Furnace::GetServerEnvelope(sent->data.data());
            REQUIRE(envelope->message_type() == ::Furnace::ServerMessage_Ack);
            auto* ack = envelope->message_as_Ack();
            REQUIRE(ack->success() == true);
        }

        TEST_CASE("StopCommand posts EvtProfileStop to FSM")
        {
            MessageHandlerTestFixture fixture;
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_, _, _));
            fixture.Init();

            // Put FSM in running state
            fixture.fsm.Post(EvtModeProfile());
            Furnace::Profile profile;
            profile.name = "TestProfile";
            profile.segments.push_back({100.0f, std::chrono::minutes(10), std::chrono::minutes(5)});
            fixture.fsm.Post(EvtProfileLoad(profile));
            fixture.fsm.Post(EvtProfileStart());
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_RUNNING);

            // Send StopCommand
            auto msg = TestClientEncoder::EncodeStopCommand(2);
            fixture.mockTransport.SimulateClientMessage(100, msg.data(), msg.size());
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_STOPPED);

            // Verify Ack
            auto* sent = fixture.mockTransport.GetLastSentMessage();
            REQUIRE(sent != nullptr);
            auto* envelope = ::Furnace::GetServerEnvelope(sent->data.data());
            auto* ack = envelope->message_as_Ack();
            REQUIRE(ack->success() == true);
        }

        TEST_CASE("LoadCommand with valid profile posts EvtProfileLoad")
        {
            MessageHandlerTestFixture fixture;
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_, _, _));
            fixture.Init();

            // Add test profile to mock loader
            auto testProfile = MockProfileLoader::CreateSimpleProfile("MyProfile", 500.0f, 30, 60);
            fixture.mockProfileLoader.AddProfile("MyProfile", testProfile);

            // Put FSM in profile mode
            fixture.fsm.Post(EvtModeProfile());
            fixture.fsm.ProcessQueue();

            // Send LoadCommand
            auto msg = TestClientEncoder::EncodeLoadCommand(3, "MyProfile");
            fixture.mockTransport.SimulateClientMessage(100, msg.data(), msg.size());
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE_LOADED);
            REQUIRE(fixture.mockProfileLoader.GetLastLoadedName() == "MyProfile");

            // Verify Ack
            auto* sent = fixture.mockTransport.GetLastSentMessage();
            auto* envelope = ::Furnace::GetServerEnvelope(sent->data.data());
            auto* ack = envelope->message_as_Ack();
            REQUIRE(ack->success() == true);
        }

        TEST_CASE("LoadCommand with unknown profile sends failure Ack")
        {
            MessageHandlerTestFixture fixture;
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_, _, _));
            fixture.Init();

            // Don't add any profiles to loader

            auto msg = TestClientEncoder::EncodeLoadCommand(4, "NonExistent");
            fixture.mockTransport.SimulateClientMessage(100, msg.data(), msg.size());

            // Verify failure Ack
            auto* sent = fixture.mockTransport.GetLastSentMessage();
            REQUIRE(sent != nullptr);
            auto* envelope = ::Furnace::GetServerEnvelope(sent->data.data());
            auto* ack = envelope->message_as_Ack();
            REQUIRE(ack->success() == false);
            REQUIRE(std::strstr(ack->error()->c_str(), "not found") != nullptr);
        }

        TEST_CASE("UnloadCommand posts EvtProfileClear")
        {
            MessageHandlerTestFixture fixture;
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_, _, _));
            fixture.Init();

            // Load a profile first
            fixture.fsm.Post(EvtModeProfile());
            Furnace::Profile profile;
            profile.name = "TestProfile";
            profile.segments.push_back({100.0f, std::chrono::minutes(10), std::chrono::minutes(5)});
            fixture.fsm.Post(EvtProfileLoad(profile));
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.IsProfileSet());

            // Send UnloadCommand
            auto msg = TestClientEncoder::EncodeUnloadCommand(5);
            fixture.mockTransport.SimulateClientMessage(100, msg.data(), msg.size());
            fixture.fsm.ProcessQueue();

            REQUIRE_FALSE(fixture.fsm.IsProfileSet());
        }

        TEST_CASE("SetTempCommand posts EvtManualSetTemp with correct temperature")
        {
            MessageHandlerTestFixture fixture;
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_, _, _));
            ALLOW_CALL(fixture.mockHeater, SetTargetTemp(_)).RETURN(true);
            fixture.Init();

            // Put FSM in manual mode
            fixture.fsm.Post(EvtModeManual());
            fixture.fsm.ProcessQueue();

            // Send SetTempCommand
            auto msg = TestClientEncoder::EncodeSetTempCommand(6, 350.5f);
            fixture.mockTransport.SimulateClientMessage(100, msg.data(), msg.size());
            fixture.fsm.ProcessQueue();

            // Verify Ack
            auto* sent = fixture.mockTransport.GetLastSentMessage();
            auto* envelope = ::Furnace::GetServerEnvelope(sent->data.data());
            auto* ack = envelope->message_as_Ack();
            REQUIRE(ack->success() == true);
        }

        TEST_CASE("SetModeCommand Off posts EvtModeOff")
        {
            MessageHandlerTestFixture fixture;
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_, _, _));
            fixture.Init();

            // Start in profile mode
            fixture.fsm.Post(EvtModeProfile());
            fixture.fsm.ProcessQueue();
            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE);

            // Send SetModeCommand(Off)
            auto msg = TestClientEncoder::EncodeSetModeCommand(7, ::Furnace::FurnaceMode_Off);
            fixture.mockTransport.SimulateClientMessage(100, msg.data(), msg.size());
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::OFF);
        }

        TEST_CASE("SetModeCommand Profile posts EvtModeProfile")
        {
            MessageHandlerTestFixture fixture;
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_, _, _));
            fixture.Init();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::OFF);

            // Send SetModeCommand(Profile)
            auto msg = TestClientEncoder::EncodeSetModeCommand(8, ::Furnace::FurnaceMode_Profile);
            fixture.mockTransport.SimulateClientMessage(100, msg.data(), msg.size());
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::PROFILE);
        }

        TEST_CASE("SetModeCommand Manual posts EvtModeManual")
        {
            MessageHandlerTestFixture fixture;
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_, _, _));
            fixture.Init();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::OFF);

            // Send SetModeCommand(Manual)
            auto msg = TestClientEncoder::EncodeSetModeCommand(9, ::Furnace::FurnaceMode_Manual);
            fixture.mockTransport.SimulateClientMessage(100, msg.data(), msg.size());
            fixture.fsm.ProcessQueue();

            REQUIRE(fixture.fsm.GetCurrentState() == StateId::MANUAL_OFF);
        }

        TEST_CASE("Invalid message sends error response")
        {
            MessageHandlerTestFixture fixture;
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_, _, _));
            fixture.Init();

            // Send garbage data
            uint8_t garbage[] = {0x01, 0x02, 0x03, 0x04, 0x05};
            fixture.mockTransport.SimulateClientMessage(100, garbage, sizeof(garbage));

            // Verify Error response
            auto* sent = fixture.mockTransport.GetLastSentMessage();
            REQUIRE(sent != nullptr);
            auto* envelope = ::Furnace::GetServerEnvelope(sent->data.data());
            REQUIRE(envelope->message_type() == ::Furnace::ServerMessage_Error);
            auto* error = envelope->message_as_Error();
            REQUIRE(error->code() == ErrorCode::BadRequest);
        }

        TEST_CASE("ListProgramsRequest returns empty program list")
        {
            MessageHandlerTestFixture fixture;
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_, _, _));
            fixture.Init();

            auto msg = TestClientEncoder::EncodeListProgramsRequest(10);
            fixture.mockTransport.SimulateClientMessage(100, msg.data(), msg.size());

            auto* sent = fixture.mockTransport.GetLastSentMessage();
            REQUIRE(sent != nullptr);
            auto* envelope = ::Furnace::GetServerEnvelope(sent->data.data());
            REQUIRE(envelope->message_type() == ::Furnace::ServerMessage_ProgramListResponse);
            REQUIRE(envelope->request_id() == 10);
        }

        TEST_CASE("GetPreferencesRequest returns placeholder JSON")
        {
            MessageHandlerTestFixture fixture;
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_, _, _));
            fixture.Init();

            auto msg = TestClientEncoder::EncodeGetPreferencesRequest(11);
            fixture.mockTransport.SimulateClientMessage(100, msg.data(), msg.size());

            auto* sent = fixture.mockTransport.GetLastSentMessage();
            REQUIRE(sent != nullptr);
            auto* envelope = ::Furnace::GetServerEnvelope(sent->data.data());
            REQUIRE(envelope->message_type() == ::Furnace::ServerMessage_PreferencesResponse);
            auto* resp = envelope->message_as_PreferencesResponse();
            REQUIRE(resp->json() != nullptr);
        }

        TEST_CASE("BroadcastState sends state to all clients")
        {
            MessageHandlerTestFixture fixture;
            ALLOW_CALL(fixture.mockLogBackend, WriteLog(_, _, _));
            fixture.Init();

            fixture.mockStateProvider.State().mode = ::Furnace::FurnaceMode_Profile;
            fixture.mockStateProvider.State().kilnTemp = 500.0f;

            fixture.handler->BroadcastState();

            auto& messages = fixture.mockTransport.GetSentMessages();
            REQUIRE(messages.size() == 1);
            REQUIRE(messages[0].isBroadcast == true);

            auto* envelope = ::Furnace::GetServerEnvelope(messages[0].data.data());
            REQUIRE(envelope->message_type() == ::Furnace::ServerMessage_State);
            auto* state = envelope->message_as_State();
            REQUIRE(state->mode() == ::Furnace::FurnaceMode_Profile);
            REQUIRE(state->kiln_temp() == doctest::Approx(500.0f));
        }
    }
} // namespace HeatTreatFurnace::Test
