#pragma once

#include "State.hpp"

class StateMachine
{
public:
    StateMachine(StateId aInitialState);
    StateId GetState() const;
    bool CanTransition(StateId aToState) const;
    bool TransitionTo(StateId aToState);
protected:
    std::map<StateId, std::shared_ptr<BaseState>> states;
    std::shared_ptr<StateMachine> instance;
private:
    StateId myCurrentState;
#include <map>
#include <set>
#include <string>

enum class State {
    Idle,
    Heating,
    Cooling,
    Error
};

std::map<StateId, std::set<StateId>> transitions = {
    {StateId::IDLE,    {StateId::LOADED, StateId::ERROR}},
    {StateId::LOADED, {StateId::RUNNING, StateId::ERROR}},
    {StateId::RUNNING, {StateId::PAUSED, StateId::COMPLETED, StateId::CANCELLED, StateId::ERROR}},
    {StateId::PAUSED,   {StateId::RUNNING, StateId::CANCELLED, StateId::ERROR}},
    {StateId::COMPLETED,   {StateId::IDLE}},
    {StateId::CANCELLED,   {StateId::IDLE}},
    {StateId::ERROR,   {StateId::IDLE}},
    {StateId::WAITING_FOR_TEMP,   {StateId::RUNNING, StateId::PAUSED, StateId::ERROR}}
};

