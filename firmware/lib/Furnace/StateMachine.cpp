#include "StateMachine.hpp"
#include "State.hpp"

StateMachine::StateMachine() :
    myCurrentState(StateId::IDLE)
{
    myInstance = std::make_shared<StateMachine>(*this);

    myStates = {
        {StateId::IDLE, std::make_shared<IdleState>(myInstance)},
        {StateId::LOADED, std::make_shared<LoadedState>(myInstance)},
        {StateId::RUNNING, std::make_shared<RunningState>(myInstance)},
        {StateId::PAUSED, std::make_shared<PausedState>(myInstance)},
        {StateId::COMPLETED, std::make_shared<CompletedState>(myInstance)},
        {StateId::CANCELLED, std::make_shared<CancelledState>(myInstance)},
        {StateId::ERROR, std::make_shared<ErrorState>(myInstance)},
        {StateId::WAITING_FOR_TEMP,
         std::make_shared<WaitingForTempState>(myInstance)}
    };
}

StateId StateMachine::GetState() const
{
    return myCurrentState;
}

bool StateMachine::CanTransition(StateId aToState) const
{
    std::set<StateId>& set = validTransitions[myCurrentState];

    if (auto search = set.find(aToState); search == set.end())
    {
        return false;
    }

    return true;
}

bool StateMachine::TransitionTo(StateId aToState)
{
    if (!CanTransition(aToState))
    {
        return false;
    }

    Result res = myStates[myCurrentState]->OnExit();
    if (!res)
    {
        //todo: Send logging command
        TransitionTo(StateId::ERROR);
    }
    myCurrentState = StateId::TRANSITIONING;

    res = myStates[aToState]->OnEnter();
    if (!res)
    {
        TransitionTo(StateId::ERROR);
    }
    myCurrentState = aToState;
    return true;
}
