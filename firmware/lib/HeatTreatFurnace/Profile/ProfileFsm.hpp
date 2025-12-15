//
// Created by chris on 12/14/25.
//

#ifndef TEST_APP_PROFILEFSM_HPP
#define TEST_APP_PROFILEFSM_HPP
#include "States.hpp"
#include "etl/fsm.h"
#include "Log/LogService.hpp"
#include "Profile.hpp"


namespace HeatTreatFurnace::Profile
{
    const etl::message_router_id_t PROFILE_FSM_ROUTER = 0;

    /**
     * Note, this class should not interact with the hardware directly. It will simply emit events to heater with updates to the target if the profile is running.
     * It will emit loaded, cleared, started, stopped, completed, etc.. events to the main FSM.
     *
     * TODO: Rather than the main FSM sending this FSM tick events, the ticks should come from the heater. The heater will
     * have it's own thread operating at a known realtime schedule anyway.
     *
     * TODO: switch the main FSM from knowing profile state (IE. running, paused, loaded, etc) to have simpler modes. a Manual Temp mode,
     * a TempProfile Temp mode, and Off.
     */
    class ProfileFsm : public etl::fsm, public Log::Loggable
    {
    public:
        enum class ProfileUpdateResult
        {
            START,
            HOLD,
            INCREASE,
            DECREASE,
            END
        };

        ProfileFsm(Log::LogService& aLogger);

        void Init();

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

    private:
        ProfileUpdateResult UpdateNextProfileTemp();
        bool ClearProgram();
        bool SetProgramPosition(int16_t aSegmentIndex, std::chrono::seconds aSegmentTimePosition);
        etl::unique_ptr<Profile> myCurrentProfile;
        Log::LogService& myLogger;
        etl::fsm_state_pack<IdleState,
                            RunningState> myStatePack;

        friend IdleState;
        friend RunningState;
    };
} // namespace HeatTreatFurnace::TempProfile


#endif //TEST_APP_PROFILEFSM_HPP
