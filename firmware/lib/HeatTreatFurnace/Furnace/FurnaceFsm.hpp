#pragma once

#include "EventQueueManager.hpp"
#include "Types.hpp"
#include "Furnace/FurnaceState.hpp"
#include "States/StateId.hpp"
#include "Log/LogService.hpp"

#include "Furnace/States/CancelledState.hpp"
#include "Furnace/States/CompletedState.hpp"
#include "Furnace/States/ErrorState.hpp"
#include "Furnace/States/IdleState.hpp"
#include "Furnace/States/LoadedState.hpp"
#include "Furnace/States/ManualTempState.hpp"
#include "Furnace/States/PausedState.hpp"
#include "Furnace/States/ProfileTempOverrideState.hpp"
#include "Furnace/States/RunningState.hpp"

#include <etl/fsm.h>

//TODO simplify this FSM to just have the main modes: Off, Manual, Profile. Profile sub-states are in the profile fsm. Manual mode can be controlled here, it's simple enough.
//TODO this should contain a reference to the Heater service, in order to turn it on and off and set the target.

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
        FurnaceFsm(FurnaceState& aFurnaceState, Log::LogService& aLogger);

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

        template <typename... Args, typename StateType>
        void SendLog(Log::LogLevel aLevel, StateType& aState, const etl::string_view& aFormat, Args&&... aArgs)
        {
            myLogService.Log(aLevel, aState.Name(), aFormat, std::forward<Args>(aArgs)...);
        }

        /**
         * @brief Get the current state ID
         * @return Current state identifier (mapped to Furnace::StateId)
         */
        [[nodiscard]] StateId GetCurrentState() const noexcept;

        /**
         * @brief Get reference to FurnaceState for states to access
         * @return Reference to furnace operational state
         */
        Furnace::FurnaceState& GetFurnaceState() noexcept { return myFurnaceState; }

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
        EventQueueManager myQueueManager;
        FurnaceState& myFurnaceState;
        Log::LogService& myLogger;
        etl::fsm_state_pack<IdleState,
                            LoadedState,
                            RunningState,
                            PausedState,
                            CompletedState,
                            CancelledState,
                            ErrorState,
                            ManualTempState,
                            ProfileTempOverrideState> myStatePack;
    };
} // namespace HeatTreatFurnace::FSM

