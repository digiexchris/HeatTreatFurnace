

#ifndef HEAT_TREAT_FURNACE_STATE_HPP
#define HEAT_TREAT_FURNACE_STATE_HPP

#include <string>
#include <set>
#include <map>
#include <memory>

#include "Result.hpp"
#include "etl/map.h"
#include "etl/string.h"

namespace HeatTreatFurnace::Furnace
{
    class Furnace;

    constexpr size_t MAX_STATE_ID_NAME_LENGTH = 16;

    using StateName = etl::string<MAX_STATE_ID_NAME_LENGTH>;

    enum class StateId : uint8_t
    {
        TRANSITIONING,
        // Invalid state, this is used between transitions. i.e. exit Idle, Enter None, Enter Error. In case the new Enter fails, it is then results as NONE.
        IDLE,
        LOADED,
        RUNNING,
        PAUSED,
        COMPLETED,
        CANCELLED,
        ERROR,
        WAITING_FOR_TEMP,
        NUM_STATES
    };

    class BaseState
    {
    public:
        virtual ~BaseState() = default;

        explicit BaseState(Furnace& aFurnace, StateId aStateId = StateId::NUM_STATES) :
            myFurnace(aFurnace), myStateId(aStateId)
        {
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

        StateName Name()
        {
            StateName name = "";
            switch (myStateId)
            {
            case StateId::IDLE:
                name = "Idle";
            case StateId::LOADED:
                name = "Loaded";
            case StateId::RUNNING:
                name = "Running";
            case StateId::PAUSED:
                name = "Paused";
            case StateId::COMPLETED:
                name = "Completed";
            case StateId::CANCELLED:
                name = "Cancelled";
            case StateId::ERROR:
                name = "Error";
            case StateId::WAITING_FOR_TEMP:
                name = "WaitingForTemp";
            default:
                break;
            }

            return name;
        }

    protected:
        Furnace& myFurnace;

        StateId myStateId;
    };

    class IdleState : public BaseState
    {
    public:
        explicit IdleState(Furnace& Furnace) :
            BaseState(Furnace, StateId::IDLE)
        {
        }

        [[nodiscard]] StateId State() const override { return StateId::IDLE; }
    };

    class LoadedState : public BaseState
    {
    public:
        explicit LoadedState(Furnace& Furnace) :
            BaseState(Furnace, StateId::LOADED)
        {
        }

        [[nodiscard]] virtual StateId State() const override { return StateId::LOADED; } // NOLINT(*-use-override)
    };

    class RunningState : public BaseState
    {
    public:
        explicit RunningState(Furnace& Furnace) :
            BaseState(Furnace, StateId::RUNNING)
        {
        }

        [[nodiscard]] virtual StateId State() const override { return StateId::RUNNING; } // NOLINT(*-use-override)
    };

    class PausedState : public BaseState
    {
    public:
        explicit PausedState(Furnace& aFurnace) :
            BaseState(aFurnace, StateId::PAUSED)
        {
        }

        [[nodiscard]] virtual StateId State() const override { return StateId::PAUSED; } // NOLINT(*-use-override)
    };

    class CompletedState : public BaseState
    {
    public:
        explicit CompletedState(Furnace& aFurnace) :
            BaseState(aFurnace, StateId::COMPLETED)
        {
        }

        [[nodiscard]] virtual StateId State() const override { return StateId::COMPLETED; } // NOLINT(*-use-override)
    };

    class CancelledState : public BaseState
    {
    public:
        explicit CancelledState(Furnace& aFurnace) :
            BaseState(aFurnace, StateId::CANCELLED)
        {
        }

        [[nodiscard]] StateId State() const override { return StateId::CANCELLED; }
    };

    class ErrorState : public BaseState
    {
    public:
        explicit ErrorState(Furnace& aFurnace) :
            BaseState(aFurnace, StateId::ERROR)
        {
        }

        [[nodiscard]] StateId State() const override { return StateId::ERROR; }
    };

    class WaitingForTempState : public BaseState
    {
    public:
        explicit WaitingForTempState(Furnace& aFurnace) :
            BaseState(aFurnace, StateId::WAITING_FOR_TEMP)
        {
        }

        [[nodiscard]] StateId State() const override { return StateId::WAITING_FOR_TEMP; }
    };
} //namespace furnace

#endif //HEAT_TREAT_FURNACE_STATE_HPP
