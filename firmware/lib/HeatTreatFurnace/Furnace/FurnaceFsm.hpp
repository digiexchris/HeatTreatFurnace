#pragma once

#include "EventQueueManager.hpp"
#include "Types.hpp"
#include "Furnace/StateId.hpp"
#include "Log/LogService.hpp"
#include "Furnace/Mode/OffState.hpp"
#include "Furnace/Mode/ManualState.hpp"
#include "Furnace/Mode/ErrorState.hpp"
#include "Furnace/Profile/Profile.hpp"
#include "Furnace/Profile/ProfileLoadedState.hpp"
#include "Furnace/Profile/ProfileRunningState.hpp"
#include "Furnace/Profile/ProfileStoppedState.hpp"
#include "Furnace/Profile/ProfileCompletedState.hpp"

#include <etl/fsm.h>

namespace HeatTreatFurnace::Furnace
{
    const etl::message_router_id_t FURNACE_FSM_ROUTER = 0;

    /**
     * @brief Main FSM class for furnace state management
     *
     * Derives from etl::fsm to handle state transitions automatically.
     * Overrides receive() to queue messages, then processes them via
     * etl::fsm::receive() during ProcessQueue().
     */
    class FurnaceFsm : public etl::fsm, public Log::Loggable
    {
    public:
        FurnaceFsm(Log::LogService& aLogger);

        /**
         * @brief Overridden receive to queue messages instead of processing immediately
         * @param aMsg Message to queue for later processing
         */
        void receive(etl::imessage const& aMsg) override;

        void Init();

        /**
         * @brief Post an event to the queue with specified priority
         * @param aMsg Event message to post
         * @param aPriority Priority level for the event
         * @return true if posted successfully, false if queue is full
         * TODO: This needs to drop all events in the queue if it is full and an ERROR comes in.
         */
        bool Post(etl::imessage const& aMsg, EventPriority aPriority);

        /**
         * @brief Process all queued events in priority order
         *
         * Drains the event queue and delivers each event to the FSM via
         * etl::fsm::receive(), which routes to current state handlers.
         * Should be called regularly from the main loop.
         */
        void ProcessQueue();

        template <typename... Args>
        void SendLog(Log::LogLevel aLevel, BaseState& aState, const etl::string_view& aFormat, Args&&... aArgs)
        {
            myLogService.Log(aLevel, aState.Name(), aFormat, std::forward<Args>(aArgs)...);
        }

        /**
         * @brief Get the current state ID
         * @return Current state identifier (mapped to Furnace::StateId)
         */
        [[nodiscard]] StateId GetCurrentState() const noexcept;

        /**
         * @brief Get reference to LogService for states to access
         * @return Reference to logging service
         */
        Log::LogService& GetLogger() noexcept { return myLogger; }

        /**
         * @brief Get the overflow counter from the queue manager
         * @return Number of events dropped due to queue overflow
         */
        [[nodiscard]] uint32_t GetOverflowCount() const noexcept;

    private:
        enum class ProfileUpdateResult
        {
            START,
            HOLD,
            INCREASE,
            DECREASE,
            END
        };

        std::shared_ptr<Profile> GetCurrentProfile() { return myCurrentProfile; }

        ProfileUpdateResult UpdateNextProfileTempTarget();
        bool ClearProgram();
        bool SetProgramPosition(int16_t aSegmentIndex, std::chrono::seconds aSegmentTimePosition);
        bool IsHeaterOn() const;
        bool SetHeaterOn();
        bool SetHeaterOff();
        bool SetHeaterTarget(float aTargetTemp);

        //This changes the next segment and segment position that will be executed by the next UpdateNextProfileTempTarget().
        bool SetCurrentProfileCurrentSegment(uint16_t aSegment, std::chrono::seconds aSegmentTimePosition);

        void HandleProfileCompleted(ProfileUpdateResult aResult, BaseState& aState)
        {
            if (aResult == FurnaceFsm::ProfileUpdateResult::END)
            {
                EvtProfileStop evt;
                Post(evt, EventPriority::Furnace);
                SendLog(Log::LogLevel::Debug, aState, "Program completed");
            }
        }

        etl::fsm_state_id_t HandleEvent(EvtManualSetTemp const& anEvent)
        {
            SetHeaterTarget(anEvent.targetTemp);
            return STATE_MANUAL;
        }

        etl::fsm_state_id_t HandleEvent(EvtProfileLoad const& anEvent)
        {
            //TODO handle the actual loading
            return STATE_PROFILE_LOADED;
        }

        std::shared_ptr<Profile> myCurrentProfile;

        EventQueueManager myQueueManager;
        Log::LogService& myLogger;
        etl::fsm_state_pack<OffState,
                            ErrorState,
                            ProfileState,
                            ProfileLoadedState,
                            ProfileRunningState,
                            ProfileCompletedState,
                            ProfileStoppedState,
                            ManualState
        > myStatePack;

        friend class ProfileRunningState;
        friend class ProfileState;
        friend class ProfileCompletedState;
        friend class ProfileLoadedState;
        friend class ManualState;
    };
} // namespace HeatTreatFurnace::FSM

