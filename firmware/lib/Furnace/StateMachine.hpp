#pragma once

#include "State.hpp"

class StateMachine
{
public:
  StateMachine();
  StateId GetState() const;
  bool CanTransition(StateId aToState) const;
  bool TransitionTo(StateId aToState);

protected:
  std::map<StateId, std::shared_ptr<BaseState>> myStates;
  std::shared_ptr<StateMachine> myInstance;

private:
  StateId myCurrentState;
};

std::map<StateId, std::set<StateId>> transitions = {
    {StateId::IDLE, {StateId::LOADED, StateId::ERROR}},
    {StateId::LOADED, {StateId::RUNNING, StateId::ERROR}},
    {StateId::RUNNING,
     {StateId::PAUSED, StateId::COMPLETED, StateId::CANCELLED, StateId::ERROR}},
    {StateId::PAUSED, {StateId::RUNNING, StateId::CANCELLED, StateId::ERROR}},
    {StateId::COMPLETED, {StateId::IDLE}},
    {StateId::CANCELLED, {StateId::IDLE}},
    {StateId::ERROR, {StateId::IDLE}},
    {StateId::WAITING_FOR_TEMP,
     {StateId::RUNNING, StateId::PAUSED, StateId::ERROR}}
};

