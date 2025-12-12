#include "StateMachine.hpp"

#include <cassert>
#include <memory>
#include "State.hpp"
#include "Log/LogService.hpp"
#include "Safety.hpp"

namespace HeatTreatFurnace::Furnace
{
    StateMachine::StateMachine(FurnaceState& aFurnace, Log::LogService& aLog) :
        Loggable(aLog),
        myLog(aLog), myFurnace(aFurnace),
        myCurrentState(StateId::IDLE)
    {

        //MISRA 21.6.1 exception:
        //Furnace is created very early on in the boot of the firmware.
        //These objects are never allocated/reallocated after this point.
        myTransitioningState = std::make_unique<TransitioningState>(aFurnace);
        myIdleState = std::make_unique<IdleState>(aFurnace);
        myLoadedState = std::make_unique<LoadedState>(aFurnace);
        myRunningState = std::make_unique<RunningState>(aFurnace);
        myPausedState = std::make_unique<PausedState>(aFurnace);
        myCompletedState = std::make_unique<CompletedState>(aFurnace);
        myCancelledState = std::make_unique<CancelledState>(aFurnace);
        myErrorState = std::make_unique<ErrorState>(aFurnace);
        myWaitingForTempState = std::make_unique<WaitingForTempState>(aFurnace);

        myStates = etl::make_map<StateId, BaseState&>(
            etl::pair{StateId::TRANSITIONING, *myTransitioningState},
            etl::pair{StateId::IDLE, *myIdleState},
            etl::pair{StateId::LOADED, *myLoadedState},
            etl::pair{StateId::RUNNING, *myRunningState},
            etl::pair{StateId::PAUSED, *myPausedState},
            etl::pair{StateId::COMPLETED, *myCompletedState},
            etl::pair{StateId::CANCELLED, *myCancelledState},
            etl::pair{StateId::ERROR, *myErrorState},
            etl::pair{StateId::WAITING_FOR_TEMP, *myWaitingForTempState}
            );

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

            Log(Log::LogLevel::Debug, "Transitioned to ERROR from {}", fromStateName);
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
            auto errorRes = TransitionTo(StateId::ERROR);
            if (!errorRes)
            {
                Log(Log::LogLevel::Error, "Failed to transition to ERROR from {}", fromStateName);
            }
            Log(Log::LogLevel::Error, "Failed to transition via {}.OnExit() to {}, {}", fromStateName, toStateName, res.message);
            return false;
        }
        myCurrentState = StateId::TRANSITIONING;

        res = myStates.at(aToState).OnEnter();
        if (!res)
        {
            auto errorRes = TransitionTo(StateId::ERROR);
            if (!errorRes)
            {
                Log(Log::LogLevel::Error, "Failed to transition to ERROR from {}", fromStateName);
            }
            Log(Log::LogLevel::Error, "Failed to transition via {}.OnEnter() from {}, {}", toStateName, fromStateName, res.message);
            return false;
        }
        myCurrentState = aToState;
        return true;
    }
} //HeatTreatFurnace::Furnace
