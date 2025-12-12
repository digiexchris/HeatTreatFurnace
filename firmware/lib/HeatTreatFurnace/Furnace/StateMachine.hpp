#ifndef HEAT_TREAT_FURNACE_STATE_MACHINE_HPP
#define HEAT_TREAT_FURNACE_STATE_MACHINE_HPP

#include "etl/set.h"
#include "Profile.hpp"
#include "State.hpp"
#include "Log/LogService.hpp"

namespace HeatTreatFurnace::Furnace
{
    class Furnace;

    class StateMachine : public Log::Loggable
    {
    public:
        constexpr static size_t NUM_STATES = static_cast<size_t>(StateId::NUM_STATES);
        using StateMap = etl::map<StateId, std::unique_ptr<BaseState>, static_cast<uint16_t>(StateId::NUM_STATES)>;

        /** @brief State Machine dependencies:
         * StateMap will be moved to myState
         */
        explicit StateMachine(FurnaceState& aFurnace, Log::LogService& aLog);
        ~StateMachine() override = default;
        [[nodiscard]] StateId GetState() const;
        [[nodiscard]] bool CanTransition(const StateId& aToState);
        bool TransitionTo(StateId aToState);

        //Actions

    protected:
        [[nodiscard]] const etl::string_view& GetLogDomain() override
        {
            return myDomain;
        };

    private:
        etl::map<StateId, BaseState&, NUM_STATES> myStates;

        // static StateMap CreateDefaultStates(Furnace* furnace);
        StateId myCurrentState;
        std::unique_ptr<Profile> myLoadedProfile;

        //The Action would have asked that the loaded profile be replaced with this, which will happen when the Load() transition happens.
        std::unique_ptr<Profile> myProfileToLoad;
        Log::LogService& myLog;

        FurnaceState& myFurnace;

        TransitioningState myTransitioningState;
        IdleState myIdleState;
        LoadedState myLoadedState;
        RunningState myRunningState;
        PausedState myPausedState;
        CompletedState myCompletedState;
        CancelledState myCancelledState;
        ErrorState myErrorState;
        WaitingForTempState myWaitingForTempState;

        static constexpr etl::string_view myDomain = "StateMachine";

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
