#pragma once

#include <string>
#include <set>
#include <map>
#include <memory>

class StateMachine;

enum class StateId
{
    NONE, // Invalid state, this is used between transitions. ie. exit Idle, Enter None, Enter Error. In case the new Enter fails, it is then results as NONE.
    IDLE,
    LOADED,
    RUNNING,
    PAUSED,
    COMPLETED,
    CANCELLED,
    ERROR,
    WAITING_FOR_TEMP
};

class BaseState
{
public:
    virtual ~BaseState() = default;
    BaseState(std::shared_ptr<StateMachine> stateMachine) { myStateMachine = stateMachine; }
    virtual StateId State() const = 0;
    virtual void OnEnter() {}
    virtual void OnLeave() {}
protected:
    std::shared_ptr<StateMachine> myStateMachine;
};

class IdleState : public BaseState
{
public:
    IdleState(std::shared_ptr<StateMachine> stateMachine) : BaseState(stateMachine) {}
    StateId State() const override { return StateId::IDLE; }
    void OnEnter() override {}
    void OnLeave() override {}
    bool CanEnter() const { return true; }
};

class LoadedState : public BaseState
{
public:
    LoadedState(std::shared_ptr<StateMachine> stateMachine) : BaseState(stateMachine) {}
    StateId State() const override { return StateId::LOADED; }
    void OnEnter() override {}
    void OnLeave() override {}
};

class RunningState : public BaseState
{
public:
    RunningState(std::shared_ptr<StateMachine> stateMachine) : BaseState(stateMachine) {}
    StateId State() const override { return StateId::RUNNING; }
    void OnEnter() override {}
    void OnLeave() override {}
};

class PausedState : public BaseState
{
public:
    PausedState(std::shared_ptr<StateMachine> stateMachine) : BaseState(stateMachine) {}
    StateId State() const override { return StateId::PAUSED; }
    void OnEnter() override {}
    void OnLeave() override {}
};

class CompletedState : public BaseState
{
public:
    CompletedState(std::shared_ptr<StateMachine> stateMachine) : BaseState(stateMachine) {}
    StateId State() const override { return StateId::COMPLETED; }
    void OnEnter() override {}
    void OnLeave() override {}
};

class CancelledState : public BaseState
{
public:
    CancelledState(std::shared_ptr<StateMachine> stateMachine) : BaseState(stateMachine) {}
    StateId State() const override { return StateId::CANCELLED; }
    void OnEnter() override {}
    void OnLeave() override {}
};

class ErrorState : public BaseState
{
public:
    ErrorState(std::shared_ptr<StateMachine> stateMachine) : BaseState(stateMachine) {}
    StateId State() const override { return StateId::ERROR; }
    void OnEnter() override {}
    void OnLeave() override {}
};

class WaitingForTempState : public BaseState
{
public:
    WaitingForTempState(std::shared_ptr<StateMachine> stateMachine) : BaseState(stateMachine) {}
    StateId State() const override { return StateId::WAITING_FOR_TEMP; }
    void OnEnter() override {}
    void OnLeave() override {}
};

struct StatePtrCmp
{
    bool operator()(const std::unique_ptr<BaseState>& lhs, const std::unique_ptr<BaseState>& rhs) const
    {
        return lhs->State() < rhs->State();
    }
};