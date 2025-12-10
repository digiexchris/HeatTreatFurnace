#ifndef HEAT_TREAT_FURNACE_STATE_MACHINE_HPP
#define HEAT_TREAT_FURNACE_STATE_MACHINE_HPP

#include "etl/set.h"
#include "Profile.hpp"
#include "State.hpp"
#include "Log/LogService.hpp"

namespace HeatTreatFurnace::Furnace
{
    class Furnace;

    class StateMachine
    {
    public:
        constexpr static size_t NUM_STATES = static_cast<size_t>(StateId::NUM_STATES);
        using StateMap = etl::map<StateId, std::unique_ptr<BaseState>, static_cast<uint16_t>(StateId::NUM_STATES)>;

        /** @brief State Machine dependencies:
         * StateMap will be moved to myState
         */
        explicit StateMachine(FurnaceState& aFurnace, Log::LogService* aLog);
        ~StateMachine() = default;
        [[nodiscard]] StateId GetState() const;
        [[nodiscard]] bool CanTransition(const StateId& aToState);
        bool TransitionTo(StateId aToState);

        //Actions

    private:
        etl::map<StateId, BaseState&, NUM_STATES> myStates;

        // static StateMap CreateDefaultStates(Furnace* furnace);
        StateId myCurrentState;
        std::unique_ptr<Profile> myLoadedProfile;

        //The Action would have asked that the loaded profile be replaced with this, which will happen when the Load() transition happens.
        std::unique_ptr<Profile> myProfileToLoad;
        Log::LogService* myLog;

        FurnaceState& myFurnace;

        std::unique_ptr<IdleState> myIdleState;
        std::unique_ptr<LoadedState> myLoadedState;
        std::unique_ptr<RunningState> myRunningState;
        std::unique_ptr<PausedState> myPausedState;
        std::unique_ptr<CompletedState> myCompletedState;
        std::unique_ptr<CancelledState> myCancelledState;
        std::unique_ptr<ErrorState> myErrorState;
        std::unique_ptr<WaitingForTempState> myWaitingForTempState;

        const etl::map<StateId, etl::set<StateId, NUM_STATES>, NUM_STATES> myValidTransitions = {
            {StateId::IDLE, {StateId::LOADED, StateId::ERROR}},
            {StateId::LOADED, {StateId::IDLE, StateId::RUNNING, StateId::ERROR}},
            {StateId::RUNNING,
             {StateId::PAUSED, StateId::COMPLETED, StateId::CANCELLED, StateId::ERROR}},
            {StateId::PAUSED, {StateId::RUNNING, StateId::CANCELLED, StateId::ERROR}},
            {StateId::COMPLETED, {StateId::IDLE, StateId::LOADED, StateId::ERROR}},
            {StateId::CANCELLED, {StateId::IDLE, StateId::LOADED, StateId::ERROR}},
            {StateId::ERROR, {StateId::IDLE, StateId::LOADED}},
            {StateId::WAITING_FOR_TEMP,
             {StateId::RUNNING, StateId::PAUSED, StateId::ERROR}}
        };
    };
} //namespace HeatTreatFurnace::Furnace

#endif //HEAT_TREAT_FURNACE_STATE_MACHINE_HPP
