#ifndef HEAT_TREAT_FURNACE_STATE_MACHINE_HPP
#define HEAT_TREAT_FURNACE_STATE_MACHINE_HPP

#include "Profile.hpp"
#include "State.hpp"

namespace HeatTreatFurnace::Furnace
{
    class Furnace;
    class LogService;

    class StateMachine
    {
    public:
        using StateMap = etl::map<StateId, std::unique_ptr<BaseState>, static_cast<uint16_t>(StateId::NUM_STATES)>;

        /** @brief State Machine dependencies:
         * StateMap will be moved to myState
         */
        explicit StateMachine(Furnace& aFurnace, LogService* aLog);
        ~StateMachine() = default;
        [[nodiscard]] StateId GetState() const;
        [[nodiscard]] bool CanTransition(const StateId& aToState);
        bool TransitionTo(StateId aToState);

        //Actions

    private:
        std::map<StateId, BaseState&> myStates;

        // static StateMap CreateDefaultStates(Furnace* furnace);
        StateId myCurrentState;
        std::unique_ptr<Profile> myLoadedProfile;

        //The Action would have asked that the loaded profile be replaced with this, which will happen when the Load() transition happens.
        std::unique_ptr<Profile> myProfileToLoad;
        LogService* myLog;

        Furnace& myFurnace;

        std::unique_ptr<IdleState> myIdleState;
        std::unique_ptr<LoadedState> myLoadedState;
        std::unique_ptr<RunningState> myRunningState;
        std::unique_ptr<PausedState> myPausedState;
        std::unique_ptr<CompletedState> myCompletedState;
        std::unique_ptr<CancelledState> myCancelledState;
        std::unique_ptr<ErrorState> myErrorState;
        std::unique_ptr<WaitingForTempState> myWaitingForTempState;

        const etl::map<StateId, std::set<StateId>, 10> myValidTransitions = {
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
