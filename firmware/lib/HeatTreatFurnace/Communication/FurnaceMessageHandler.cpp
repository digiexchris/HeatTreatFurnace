#include "FurnaceMessageHandler.hpp"
#include "Furnace/Events.hpp"

namespace HeatTreatFurnace::Communication
{
    FurnaceMessageHandler::FurnaceMessageHandler(
        Furnace::FurnaceFsm& aFsm,
        ITransport& aTransport,
        IStateProvider& aStateProvider,
        IProfileLoader& aProfileLoader,
        Log::LogService& aLogger)
        : myFsm(aFsm)
          , myTransport(aTransport)
          , myStateProvider(aStateProvider)
          , myProfileLoader(aProfileLoader)
          , myLogger(aLogger)
    {
    }

    void FurnaceMessageHandler::HandleMessage(uint32_t aClientId, const uint8_t* aData, size_t aLen)
    {
        auto decoded = MessageCodec::DecodeClientEnvelope(aData, aLen);
        if (!decoded.isValid)
        {
            myLogger.Log(Log::LogLevel::Warn, "MsgHandler", "Invalid message received from client {}", aClientId);
            PrivSendError(0, aClientId, ErrorCode::BadRequest, "Invalid message format");
            return;
        }

        const auto requestId = decoded.requestId;
        const auto* envelope = decoded.envelope;

        switch (decoded.messageType)
        {
        case ::Furnace::ClientMessage_StartCommand:
            PrivHandleStartCommand(requestId, aClientId, envelope->message_as_StartCommand());
            break;

        case ::Furnace::ClientMessage_StopCommand:
            PrivHandleStopCommand(requestId, aClientId);
            break;

        case ::Furnace::ClientMessage_LoadCommand:
            PrivHandleLoadCommand(requestId, aClientId, envelope->message_as_LoadCommand());
            break;

        case ::Furnace::ClientMessage_UnloadCommand:
            PrivHandleUnloadCommand(requestId, aClientId);
            break;

        case ::Furnace::ClientMessage_SetTempCommand:
            PrivHandleSetTempCommand(requestId, aClientId, envelope->message_as_SetTempCommand());
            break;

        case ::Furnace::ClientMessage_SetModeCommand:
            PrivHandleSetModeCommand(requestId, aClientId, envelope->message_as_SetModeCommand());
            break;

        case ::Furnace::ClientMessage_SetTimeScaleCommand:
            PrivHandleSetTimeScaleCommand(requestId, aClientId, envelope->message_as_SetTimeScaleCommand());
            break;

        case ::Furnace::ClientMessage_HistoryRequest:
            PrivHandleHistoryRequest(requestId, aClientId, envelope->message_as_HistoryRequest());
            break;

        case ::Furnace::ClientMessage_ListProgramsRequest:
            PrivHandleListProgramsRequest(requestId, aClientId);
            break;

        case ::Furnace::ClientMessage_GetProgramRequest:
            PrivHandleGetProgramRequest(requestId, aClientId, envelope->message_as_GetProgramRequest());
            break;

        case ::Furnace::ClientMessage_SaveProgramRequest:
            PrivHandleSaveProgramRequest(requestId, aClientId, envelope->message_as_SaveProgramRequest());
            break;

        case ::Furnace::ClientMessage_DeleteProgramRequest:
            PrivHandleDeleteProgramRequest(requestId, aClientId, envelope->message_as_DeleteProgramRequest());
            break;

        case ::Furnace::ClientMessage_GetPreferencesRequest:
            PrivHandleGetPreferencesRequest(requestId, aClientId);
            break;

        case ::Furnace::ClientMessage_SavePreferencesRequest:
            PrivHandleSavePreferencesRequest(requestId, aClientId, envelope->message_as_SavePreferencesRequest());
            break;

        case ::Furnace::ClientMessage_GetDebugInfoRequest:
            PrivHandleGetDebugInfoRequest(requestId, aClientId);
            break;

        case ::Furnace::ClientMessage_ListLogsRequest:
            PrivHandleListLogsRequest(requestId, aClientId);
            break;

        case ::Furnace::ClientMessage_GetLogRequest:
            PrivHandleGetLogRequest(requestId, aClientId, envelope->message_as_GetLogRequest());
            break;

        default:
            myLogger.Log(Log::LogLevel::Warn, "MsgHandler", "Unknown message type: {}",
                         static_cast<int>(decoded.messageType));
            PrivSendError(requestId, aClientId, ErrorCode::BadRequest, "Unknown message type");
            break;
        }
    }

    void FurnaceMessageHandler::BroadcastState()
    {
        auto stateData = myStateProvider.GetCurrentState();
        auto size = MessageCodec::EncodeState(0, stateData, myEncodeBuffer);
        if (size > 0)
        {
            myTransport.Broadcast(myEncodeBuffer.data(), size);
        }
    }

    void FurnaceMessageHandler::PrivHandleStartCommand(uint32_t aRequestId, uint32_t aClientId, const ::Furnace::StartCommand* aCmd)
    {
        myLogger.Log(Log::LogLevel::Info, "MsgHandler", "StartCommand received");

        if (aCmd != nullptr)
        {
            int segment = aCmd->segment();
            int minute = aCmd->minute();

            if (segment > 0 || minute > 0)
            {
                Furnace::EvtProfileSetNextSegment evt(
                    static_cast<uint16_t>(segment),
                    std::chrono::seconds(minute * 60));
                myFsm.Post(evt);
            }
        }

        Furnace::EvtProfileStart evt;
        myFsm.Post(evt);
        PrivSendAck(aRequestId, aClientId, true);
    }

    void FurnaceMessageHandler::PrivHandleStopCommand(uint32_t aRequestId, uint32_t aClientId)
    {
        myLogger.Log(Log::LogLevel::Info, "MsgHandler", "StopCommand received");

        Furnace::EvtProfileStop evt;
        myFsm.Post(evt);
        PrivSendAck(aRequestId, aClientId, true);
    }

    void FurnaceMessageHandler::PrivHandleLoadCommand(uint32_t aRequestId, uint32_t aClientId, const ::Furnace::LoadCommand* aCmd)
    {
        if (aCmd == nullptr || aCmd->program() == nullptr)
        {
            PrivSendAck(aRequestId, aClientId, false, "Missing program name");
            return;
        }

        const char* programName = aCmd->program()->c_str();
        myLogger.Log(Log::LogLevel::Info, "MsgHandler", "LoadCommand received for: {}", programName);

        Furnace::Profile profile;
        if (!myProfileLoader.LoadProfile(programName, profile))
        {
            PrivSendAck(aRequestId, aClientId, false, "Profile not found");
            return;
        }

        Furnace::EvtProfileLoad evt(profile);
        myFsm.Post(evt);
        PrivSendAck(aRequestId, aClientId, true);
    }

    void FurnaceMessageHandler::PrivHandleUnloadCommand(uint32_t aRequestId, uint32_t aClientId)
    {
        myLogger.Log(Log::LogLevel::Info, "MsgHandler", "UnloadCommand received");

        Furnace::EvtProfileClear evt;
        myFsm.Post(evt);
        PrivSendAck(aRequestId, aClientId, true);
    }

    void FurnaceMessageHandler::PrivHandleSetTempCommand(uint32_t aRequestId, uint32_t aClientId, const ::Furnace::SetTempCommand* aCmd)
    {
        if (aCmd == nullptr)
        {
            PrivSendAck(aRequestId, aClientId, false, "Invalid SetTempCommand");
            return;
        }

        float temp = aCmd->temperature();
        myLogger.Log(Log::LogLevel::Info, "MsgHandler", "SetTempCommand received: {} C", temp);

        Furnace::EvtManualSetTemp evt(temp);
        myFsm.Post(evt);
        PrivSendAck(aRequestId, aClientId, true);
    }

    void FurnaceMessageHandler::PrivHandleSetModeCommand(uint32_t aRequestId, uint32_t aClientId, const ::Furnace::SetModeCommand* aCmd)
    {
        if (aCmd == nullptr)
        {
            PrivSendAck(aRequestId, aClientId, false, "Invalid SetModeCommand");
            return;
        }

        auto mode = aCmd->mode();
        myLogger.Log(Log::LogLevel::Info, "MsgHandler", "SetModeCommand received: {}",
                     ::Furnace::EnumNameFurnaceMode(mode));

        switch (mode)
        {
        case ::Furnace::FurnaceMode_Off:
            {
                Furnace::EvtModeOff evt;
                myFsm.Post(evt);
                break;
            }
        case ::Furnace::FurnaceMode_Profile:
            {
                Furnace::EvtModeProfile evt;
                myFsm.Post(evt);
                break;
            }
        case ::Furnace::FurnaceMode_Manual:
            {
                Furnace::EvtModeManual evt;
                myFsm.Post(evt);
                break;
            }
        case ::Furnace::FurnaceMode_Error:
        default:
            PrivSendAck(aRequestId, aClientId, false, "Cannot manually set Error mode");
            return;
        }

        PrivSendAck(aRequestId, aClientId, true);
    }

    void FurnaceMessageHandler::PrivHandleSetTimeScaleCommand(uint32_t aRequestId, uint32_t aClientId, const ::Furnace::SetTimeScaleCommand* aCmd)
    {
        // Simulator-only command - not implemented in firmware
        myLogger.Log(Log::LogLevel::Debug, "MsgHandler", "SetTimeScaleCommand ignored (simulator only)");
        PrivSendAck(aRequestId, aClientId, true);
    }

    void FurnaceMessageHandler::PrivHandleHistoryRequest(uint32_t aRequestId, uint32_t aClientId, const ::Furnace::HistoryRequest* aReq)
    {
        // TODO: Implement history retrieval
        myLogger.Log(Log::LogLevel::Debug, "MsgHandler", "HistoryRequest - not implemented");
        PrivSendError(aRequestId, aClientId, ErrorCode::NotImplemented, "History not implemented");
    }

    void FurnaceMessageHandler::PrivHandleListProgramsRequest(uint32_t aRequestId, uint32_t aClientId)
    {
        // TODO: Implement program listing from storage
        myLogger.Log(Log::LogLevel::Debug, "MsgHandler", "ListProgramsRequest - returning empty list");
        auto size = MessageCodec::EncodeProgramListResponse(aRequestId, myEncodeBuffer);
        if (size > 0)
        {
            myTransport.Send(aClientId, myEncodeBuffer.data(), size);
        }
    }

    void FurnaceMessageHandler::PrivHandleGetProgramRequest(uint32_t aRequestId, uint32_t aClientId, const ::Furnace::GetProgramRequest* aReq)
    {
        // TODO: Implement program retrieval from storage
        myLogger.Log(Log::LogLevel::Debug, "MsgHandler", "GetProgramRequest - not implemented");
        PrivSendError(aRequestId, aClientId, ErrorCode::NotImplemented, "GetProgram not implemented");
    }

    void FurnaceMessageHandler::PrivHandleSaveProgramRequest(uint32_t aRequestId, uint32_t aClientId, const ::Furnace::SaveProgramRequest* aReq)
    {
        // TODO: Implement program saving to storage
        myLogger.Log(Log::LogLevel::Debug, "MsgHandler", "SaveProgramRequest - not implemented");
        PrivSendError(aRequestId, aClientId, ErrorCode::NotImplemented, "SaveProgram not implemented");
    }

    void FurnaceMessageHandler::PrivHandleDeleteProgramRequest(uint32_t aRequestId, uint32_t aClientId, const ::Furnace::DeleteProgramRequest* aReq)
    {
        // TODO: Implement program deletion from storage
        myLogger.Log(Log::LogLevel::Debug, "MsgHandler", "DeleteProgramRequest - not implemented");
        PrivSendError(aRequestId, aClientId, ErrorCode::NotImplemented, "DeleteProgram not implemented");
    }

    void FurnaceMessageHandler::PrivHandleGetPreferencesRequest(uint32_t aRequestId, uint32_t aClientId)
    {
        // Return placeholder preferences
        myLogger.Log(Log::LogLevel::Debug, "MsgHandler", "GetPreferencesRequest - returning placeholder");
        static const char* placeholderPrefs = R"({"placeholder": true})";
        auto size = MessageCodec::EncodePreferencesResponse(aRequestId, placeholderPrefs, myEncodeBuffer);
        if (size > 0)
        {
            myTransport.Send(aClientId, myEncodeBuffer.data(), size);
        }
    }

    void FurnaceMessageHandler::PrivHandleSavePreferencesRequest(uint32_t aRequestId, uint32_t aClientId, const ::Furnace::SavePreferencesRequest* aReq)
    {
        // TODO: Implement preferences saving
        myLogger.Log(Log::LogLevel::Debug, "MsgHandler", "SavePreferencesRequest - not implemented");
        PrivSendAck(aRequestId, aClientId, true); // Pretend success
    }

    void FurnaceMessageHandler::PrivHandleGetDebugInfoRequest(uint32_t aRequestId, uint32_t aClientId)
    {
        // Return placeholder debug info
        myLogger.Log(Log::LogLevel::Debug, "MsgHandler", "GetDebugInfoRequest - returning placeholder");
        static const char* placeholderDebug = R"({"version": "1.0.0", "uptime_ms": 0})";
        auto size = MessageCodec::EncodeDebugInfoResponse(aRequestId, placeholderDebug, myEncodeBuffer);
        if (size > 0)
        {
            myTransport.Send(aClientId, myEncodeBuffer.data(), size);
        }
    }

    void FurnaceMessageHandler::PrivHandleListLogsRequest(uint32_t aRequestId, uint32_t aClientId)
    {
        // TODO: Implement log listing
        myLogger.Log(Log::LogLevel::Debug, "MsgHandler", "ListLogsRequest - not implemented");
        PrivSendError(aRequestId, aClientId, ErrorCode::NotImplemented, "ListLogs not implemented");
    }

    void FurnaceMessageHandler::PrivHandleGetLogRequest(uint32_t aRequestId, uint32_t aClientId, const ::Furnace::GetLogRequest* aReq)
    {
        // TODO: Implement log retrieval
        myLogger.Log(Log::LogLevel::Debug, "MsgHandler", "GetLogRequest - not implemented");
        PrivSendError(aRequestId, aClientId, ErrorCode::NotImplemented, "GetLog not implemented");
    }

    void FurnaceMessageHandler::PrivSendAck(uint32_t aRequestId, uint32_t aClientId, bool aSuccess, const char* aError)
    {
        auto size = MessageCodec::EncodeAck(aRequestId, aSuccess, aError, myEncodeBuffer);
        if (size > 0)
        {
            myTransport.Send(aClientId, myEncodeBuffer.data(), size);
        }
    }

    void FurnaceMessageHandler::PrivSendError(uint32_t aRequestId, uint32_t aClientId, int32_t aCode, const char* aMessage)
    {
        auto size = MessageCodec::EncodeError(aRequestId, aCode, aMessage, myEncodeBuffer);
        if (size > 0)
        {
            myTransport.Send(aClientId, myEncodeBuffer.data(), size);
        }
    }
} // namespace HeatTreatFurnace::Communication
