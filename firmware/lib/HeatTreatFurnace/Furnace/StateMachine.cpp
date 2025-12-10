#include "StateMachine.hpp"

#include <cassert>
#include <memory>
#include "State.hpp"
#include "Log/LogService.hpp"
#include "Safety.hpp"

namespace HeatTreatFurnace::Furnace
{
    StateMachine::StateMachine(FurnaceState& aFurnace, Log::LogService* aLog) :
        myLog(aLog), myFurnace(aFurnace),
        myCurrentState(StateId::IDLE)
    {
        assert(aLog != nullptr);

        //MISRA 21.6.1 exception:
        //Furnace is created very early on in the boot of the firmware.
        //These objects are never allocated/reallocated after this point.
        myIdleState = std::make_unique<IdleState>(aFurnace);
        myLoadedState = std::make_unique<LoadedState>(aFurnace);
        myRunningState = std::make_unique<RunningState>(aFurnace);
        myPausedState = std::make_unique<PausedState>(aFurnace);
        myCompletedState = std::make_unique<CompletedState>(aFurnace);
        myCancelledState = std::make_unique<CancelledState>(aFurnace);
        myErrorState = std::make_unique<ErrorState>(aFurnace);
        myWaitingForTempState = std::make_unique<WaitingForTempState>(aFurnace);

        auto result = myStates.emplace(StateId::IDLE, *myIdleState);
        assert(result.second);
        result = myStates.emplace(StateId::LOADED, *myLoadedState);
        assert(result.second);
        result = myStates.emplace(StateId::RUNNING, *myRunningState);
        assert(result.second);
        result = myStates.emplace(StateId::PAUSED, *myPausedState);
        assert(result.second);
        result = myStates.emplace(StateId::COMPLETED, *myCompletedState);
        assert(result.second);
        result = myStates.emplace(StateId::CANCELLED, *myCancelledState);
        assert(result.second);
        result = myStates.emplace(StateId::ERROR, *myErrorState);
        assert(result.second);
        result = myStates.emplace(StateId::WAITING_FOR_TEMP, *myWaitingForTempState);
        assert(result.second);
    }

    StateId StateMachine::GetState() const
    {
        return myCurrentState;
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    bool StateMachine::CanTransition(const StateId& aToState)
    {
        if (aToState == StateId::ERROR)
        {
            return true;
        }

        const etl::set<StateId, NUM_STATES>& set = myValidTransitions.at(myCurrentState);

        if (auto search = set.find(aToState); search == set.end())
        {
            return false;
        }

        return true;
    }

    bool StateMachine::TransitionTo(StateId aToState)
    {
        StateName fromStateName = myStates.at(myCurrentState).Name();
        StateName toStateName = myStates.at(aToState).Name();
        //Safety reset on ERROR, so always allow the transition
        if (aToState == StateId::ERROR)
        {
            auto result = myStates.at(StateId::ERROR).OnEnter();
            DISCARD(result);

            myLog->Log(Log::LogLevel::Debug, "StateMachine", "Transitioned to ERROR from {}", fromStateName);
            return true;
        }

        if (!CanTransition(aToState))
        {
            //todo: Send logging command
            return false;
        }

        Result res = myStates.at(myCurrentState).OnExit();
        if (!res)
        {
            //todo: Send logging command
            auto errorRes = TransitionTo(StateId::ERROR);
            if (!errorRes)
            {
                myLog->Log(Log::LogLevel::Error, "StateMachine", "Failed to transition to ERROR from {}", fromStateName);
            }
            myLog->Log(Log::LogLevel::Error, "StateMachine", "Failed to transition from {} to {}, {}", fromStateName, toStateName, res.message);
            return false;
        }
        myCurrentState = StateId::TRANSITIONING;

        res = myStates[aToState].OnEnter();
        if (!res)
        {
            //todo: Send logging command
            TransitionTo(StateId::ERROR);
            return false;
        }
        myCurrentState = aToState;
        return true;
    }
} //HeatTreatFurnace::Furnace
