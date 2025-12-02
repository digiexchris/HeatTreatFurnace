#include "StateMachine.hpp"
#include "State.hpp"

StateMachine::StateMachine(StateId aInitialState)
    : myCurrentState(aInitialState)
{
    instance = std::make_shared<StateMachine>(*this);

   states = {
    {StateId::IDLE,std::make_shared<IdleState>(instance)},
    {StateId::LOADED,std::make_shared<LoadedState>(instance)},
    {StateId::RUNNING,std::make_shared<RunningState>(instance)},
    {StateId::PAUSED,std::make_shared<PausedState>(instance)},
    {StateId::COMPLETED,std::make_shared<CompletedState>(instance)},
    {StateId::CANCELLED,std::make_shared<CancelledState>(instance)},
    {StateId::ERROR,std::make_shared<ErrorState>(instance)},
    {StateId::WAITING_FOR_TEMP,std::make_shared<WaitingForTempState>(instance)}
   }
}

StateId StateMachine::GetState() const
{
    return myCurrentState;
}

bool StateMachine::CanTransition(State aToState) const
{
    auto it = transutuins.find(myCurrentState);
    if (it == transutuins.end())
    {
        return false;
    }
    if (!it->second.contains(aToState))
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
    myCurrentState = aToState;
    return true;
}
