#include "StateMachine.hpp"

#include <cassert>
#include <memory>
#include <utility>
#include "State.hpp"
#include "Log/LogService.hpp"

StateMachine::StateMachine(Furnace* aFurnace, LogService* aLog, StateMap aStateMap) :
    myLog(aLog), myStates(std::move(aStateMap)), myCurrentState(StateId::IDLE)
{
    assert(aLog != nullptr);

    if (myStates.empty())
    {
        StateMap states = std::move(CreateDefaultStates(aFurnace));
        myStates.swap(states);

        assert(!myStates.empty());
    }
}

StateId StateMachine::GetState() const
{
    return myCurrentState;
}

StateMachine::StateMap StateMachine::CreateDefaultStates(Furnace* furnace)
{
    StateMap states;
    states.insert({StateId::IDLE, std::make_unique<IdleState>(furnace)});
    states.insert({StateId::IDLE, std::make_unique<IdleState>(furnace)});
    states.insert({StateId::LOADED, std::make_unique<LoadedState>(furnace)});
    states.insert({StateId::RUNNING, std::make_unique<RunningState>(furnace)});
    states.insert({StateId::PAUSED, std::make_unique<PausedState>(furnace)});
    states.insert({StateId::COMPLETED, std::make_unique<CompletedState>(furnace)});
    states.insert({StateId::CANCELLED, std::make_unique<CancelledState>(furnace)});
    states.insert({StateId::ERROR, std::make_unique<ErrorState>(furnace)});
    states.insert({StateId::WAITING_FOR_TEMP, std::make_unique<WaitingForTempState>(furnace)});
    return states;
}

// ReSharper disable once CppMemberFunctionMayBeConst
bool StateMachine::CanTransition(const StateId& aToState)
{
    if (aToState == StateId::ERROR)
    {
        return true;
    }

    std::set<StateId>& set = validTransitions[myCurrentState];

    if (auto search = set.find(aToState); search == set.end())
    {
        return false;
    }

    return true;
}

bool StateMachine::TransitionTo(StateId aToState)
{
    std::string fromState = ToString(myCurrentState);
    //Safety reset on ERROR, so always allow the transition
    if (aToState == StateId::ERROR)
    {
        myStates[StateId::ERROR]->OnEnter();
        myLog->Log(LogLevel::Debug, "StateMachine", "Transitioned to ERROR from {}", fromState);
        return true;
    }

    if (!CanTransition(aToState))
    {
        //todo: Send logging command
        return false;
    }

    Result res = myStates[myCurrentState]->OnExit();
    if (!res)
    {
        //todo: Send logging command
        TransitionTo(StateId::ERROR);
        return false;
    }
    myCurrentState = StateId::TRANSITIONING;

    res = myStates[aToState]->OnEnter();
    if (!res)
    {
        //todo: Send logging command
        TransitionTo(StateId::ERROR);
        return false;
    }
    myCurrentState = aToState;
    return true;
}
