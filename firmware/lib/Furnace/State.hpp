#pragma once

#include <string>
#include <set>
#include <map>
#include <memory>

#include "Result.hpp"

class StateMachine;

enum class StateId
{
  TRANSITIONING,
  // Invalid state, this is used between transitions. ie. exit Idle, Enter None, Enter Error. In case the new Enter fails, it is then results as NONE.
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

  BaseState(std::shared_ptr<StateMachine> stateMachine)
  {
    myStateMachine = stateMachine;
  }

  virtual StateId State() const = 0;

  virtual Result OnEnter()
  {
    return {true, ""};
  }

  virtual Result OnExit()
  {
    return {true, ""};
  }

protected:
  std::shared_ptr<StateMachine> myStateMachine;
};

class IdleState : public BaseState
{
public:
  IdleState(std::shared_ptr<StateMachine> stateMachine) : BaseState(
      stateMachine)
  {
  }

  StateId State() const override { return StateId::IDLE; }

  bool CanEnter() const { return true; }
};

class LoadedState : public BaseState
{
public:
  LoadedState(std::shared_ptr<StateMachine> stateMachine) : BaseState(
      stateMachine)
  {
  }

  StateId State() const override { return StateId::LOADED; }
};

class RunningState : public BaseState
{
public:
  RunningState(std::shared_ptr<StateMachine> stateMachine) : BaseState(
      stateMachine)
  {
  }

  StateId State() const override { return StateId::RUNNING; }
};

class PausedState : public BaseState
{
public:
  PausedState(std::shared_ptr<StateMachine> stateMachine) : BaseState(
      stateMachine)
  {
  }

  StateId State() const override { return StateId::PAUSED; }
};

class CompletedState : public BaseState
{
public:
  CompletedState(std::shared_ptr<StateMachine> stateMachine) : BaseState(
      stateMachine)
  {
  }

  StateId State() const override { return StateId::COMPLETED; }
};

class CancelledState : public BaseState
{
public:
  CancelledState(std::shared_ptr<StateMachine> stateMachine) : BaseState(
      stateMachine)
  {
  }

  StateId State() const override { return StateId::CANCELLED; }
};

class ErrorState : public BaseState
{
public:
  ErrorState(std::shared_ptr<StateMachine> stateMachine) : BaseState(
      stateMachine)
  {
  }

  StateId State() const override { return StateId::ERROR; }
};

class WaitingForTempState : public BaseState
{
public:
  WaitingForTempState(std::shared_ptr<StateMachine> stateMachine) : BaseState(
      stateMachine)
  {
  }

  StateId State() const override { return StateId::WAITING_FOR_TEMP; }
};