#pragma once

#include "IMessageHandler.hpp"
#include "ITransport.hpp"
#include "IStateProvider.hpp"
#include "IProfileLoader.hpp"
#include "MessageCodec.hpp"
#include "Furnace/FurnaceFsm.hpp"
#include "Log/LogService.hpp"

namespace HeatTreatFurnace::Communication
{
    namespace ErrorCode
    {
        constexpr int32_t BadRequest = 400;
        constexpr int32_t NotImplemented = 501;
    }

    /**
     * @brief Message handler that bridges FlatBuffers messages to FSM events
     *
     * Decodes incoming ClientEnvelope messages and posts appropriate events
     * to the FurnaceFsm. Sends Ack/Error responses via the transport.
     */
    class FurnaceMessageHandler : public IMessageHandler
    {
    public:
        FurnaceMessageHandler(
            Furnace::FurnaceFsm& aFsm,
            ITransport& aTransport,
            IStateProvider& aStateProvider,
            IProfileLoader& aProfileLoader,
            Log::LogService& aLogger);

        void HandleMessage(uint32_t aClientId, const uint8_t* aData, size_t aLen) override;

        /**
         * @brief Broadcast current state to all clients
         *
         * Call this periodically or when state changes to update clients.
         */
        void BroadcastState();

    private:
        void PrivHandleStartCommand(uint32_t aRequestId, uint32_t aClientId, const ::Furnace::StartCommand* aCmd);
        void PrivHandleStopCommand(uint32_t aRequestId, uint32_t aClientId);
        void PrivHandleLoadCommand(uint32_t aRequestId, uint32_t aClientId, const ::Furnace::LoadCommand* aCmd);
        void PrivHandleUnloadCommand(uint32_t aRequestId, uint32_t aClientId);
        void PrivHandleSetTempCommand(uint32_t aRequestId, uint32_t aClientId, const ::Furnace::SetTempCommand* aCmd);
        void PrivHandleSetModeCommand(uint32_t aRequestId, uint32_t aClientId, const ::Furnace::SetModeCommand* aCmd);
        void PrivHandleSetTimeScaleCommand(uint32_t aRequestId, uint32_t aClientId, const ::Furnace::SetTimeScaleCommand* aCmd);

        void PrivHandleHistoryRequest(uint32_t aRequestId, uint32_t aClientId, const ::Furnace::HistoryRequest* aReq);
        void PrivHandleListProgramsRequest(uint32_t aRequestId, uint32_t aClientId);
        void PrivHandleGetProgramRequest(uint32_t aRequestId, uint32_t aClientId, const ::Furnace::GetProgramRequest* aReq);
        void PrivHandleSaveProgramRequest(uint32_t aRequestId, uint32_t aClientId, const ::Furnace::SaveProgramRequest* aReq);
        void PrivHandleDeleteProgramRequest(uint32_t aRequestId, uint32_t aClientId, const ::Furnace::DeleteProgramRequest* aReq);
        void PrivHandleGetPreferencesRequest(uint32_t aRequestId, uint32_t aClientId);
        void PrivHandleSavePreferencesRequest(uint32_t aRequestId, uint32_t aClientId, const ::Furnace::SavePreferencesRequest* aReq);
        void PrivHandleGetDebugInfoRequest(uint32_t aRequestId, uint32_t aClientId);
        void PrivHandleListLogsRequest(uint32_t aRequestId, uint32_t aClientId);
        void PrivHandleGetLogRequest(uint32_t aRequestId, uint32_t aClientId, const ::Furnace::GetLogRequest* aReq);

        void PrivSendAck(uint32_t aRequestId, uint32_t aClientId, bool aSuccess, const char* aError = nullptr);
        void PrivSendError(uint32_t aRequestId, uint32_t aClientId, int32_t aCode, const char* aMessage);

        Furnace::FurnaceFsm& myFsm;
        ITransport& myTransport;
        IStateProvider& myStateProvider;
        IProfileLoader& myProfileLoader;
        Log::LogService& myLogger;
        etl::vector<uint8_t, MAX_MESSAGE_SIZE> myEncodeBuffer;
    };
} // namespace HeatTreatFurnace::Communication
