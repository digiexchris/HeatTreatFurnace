#pragma once

#include <string>
#include <set>
#include <map>
#include <memory>

#include "Result.hpp"

class Furnace;

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

inline std::map<StateId, std::set<StateId>> validTransitions = {
    {StateId::IDLE, {StateId::LOADED, StateId::ERROR}},
    {StateId::LOADED, {StateId::RUNNING, StateId::ERROR}},
    {StateId::RUNNING,
     {StateId::PAUSED, StateId::COMPLETED, StateId::CANCELLED, StateId::ERROR}},
    {StateId::PAUSED, {StateId::RUNNING, StateId::CANCELLED, StateId::ERROR}},
    {StateId::COMPLETED, {StateId::IDLE, StateId::LOADED}},
    {StateId::CANCELLED, {StateId::IDLE, StateId::LOADED}},
    {StateId::ERROR, {StateId::IDLE, StateId::LOADED}},
    {StateId::WAITING_FOR_TEMP,
     {StateId::RUNNING, StateId::PAUSED, StateId::ERROR}}
};

class BaseState
{
public:
    virtual ~BaseState() = default;

    explicit BaseState(Furnace* Furnace)
    {
        myFurnace = Furnace;
    }

    [[nodiscard]] virtual StateId State() const = 0;

    virtual Result OnEnter()
    {
        return {true, ""};
    }

    virtual Result OnExit()
    {
        return {true, ""};
    }

protected:
    Furnace* myFurnace;
};

class IdleState : public BaseState
{
public:
    explicit IdleState(Furnace* Furnace) :
        BaseState(
            Furnace)
    {
    }

    [[nodiscard]] StateId State() const override { return StateId::IDLE; }
};

class LoadedState : public BaseState
{
public:
    explicit LoadedState(Furnace* Furnace) :
        BaseState(
            Furnace)
    {
    }

    [[nodiscard]] StateId State() const override { return StateId::LOADED; }
};

class RunningState : public BaseState
{
public:
    explicit RunningState(Furnace* Furnace) :
        BaseState(
            Furnace)
    {
    }

    [[nodiscard]] StateId State() const override { return StateId::RUNNING; }
};

class PausedState : public BaseState
{
public:
    explicit PausedState(Furnace* Furnace) :
        BaseState(
            Furnace)
    {
    }

    [[nodiscard]] StateId State() const override { return StateId::PAUSED; }
};

class CompletedState : public BaseState
{
public:
    explicit CompletedState(Furnace* Furnace) :
        BaseState(
            Furnace)
    {
    }

    [[nodiscard]] StateId State() const override { return StateId::COMPLETED; }
};

class CancelledState : public BaseState
{
public:
    explicit CancelledState(Furnace* Furnace) :
        BaseState(
            Furnace)
    {
    }

    [[nodiscard]] StateId State() const override { return StateId::CANCELLED; }
};

class ErrorState : public BaseState
{
public:
    explicit ErrorState(Furnace* Furnace) :
        BaseState(
            Furnace)
    {
    }

    [[nodiscard]] StateId State() const override { return StateId::ERROR; }
};

class WaitingForTempState : public BaseState
{
public:
    explicit WaitingForTempState(Furnace* Furnace) :
        BaseState(
            Furnace)
    {
    }

    [[nodiscard]] StateId State() const override { return StateId::WAITING_FOR_TEMP; }
};
