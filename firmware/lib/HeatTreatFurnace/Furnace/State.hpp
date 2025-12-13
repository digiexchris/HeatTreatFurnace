

#ifndef HEAT_TREAT_FURNACE_STATE_HPP
#define HEAT_TREAT_FURNACE_STATE_HPP

#include <string>

#include "Result.hpp"
#include "etl/map.h"
#include "etl/string.h"

namespace HeatTreatFurnace::Furnace
{
    class FurnaceState;

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
        MANUAL_TEMP,
        PROFILE_TEMP_OVERRIDE,
        NUM_STATES
    };

    class BaseState
    {
    public:
        virtual ~BaseState() = default;

        explicit BaseState(FurnaceState& aFurnace, StateId aStateId = StateId::NUM_STATES) :
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
        FurnaceState& myFurnace;

        StateId myStateId;
    };

    class TransitioningState : public BaseState
    {
    public:
        explicit TransitioningState(FurnaceState& aFurnace) :
            BaseState(aFurnace, StateId::TRANSITIONING)
        {
        }

        [[nodiscard]] StateId State() const override { return StateId::TRANSITIONING; }
    };

    class IdleState : public BaseState
    {
    public:
        explicit IdleState(FurnaceState& aFurnace) :
            BaseState(aFurnace, StateId::IDLE)
        {
        }

        [[nodiscard]] StateId State() const override { return StateId::IDLE; }
    };

    class LoadedState : public BaseState
    {
    public:
        explicit LoadedState(FurnaceState& Furnace) :
            BaseState(Furnace, StateId::LOADED)
        {
        }

        [[nodiscard]] StateId State() const override { return StateId::LOADED; }
    };

    class RunningState : public BaseState
    {
    public:
        explicit RunningState(FurnaceState& Furnace) :
            BaseState(Furnace, StateId::RUNNING)
        {
        }

        [[nodiscard]] StateId State() const override { return StateId::RUNNING; }
    };

    class PausedState : public BaseState
    {
    public:
        explicit PausedState(FurnaceState& aFurnace) :
            BaseState(aFurnace, StateId::PAUSED)
        {
        }

        [[nodiscard]] StateId State() const override { return StateId::PAUSED; }
    };

    class CompletedState : public BaseState
    {
    public:
        explicit CompletedState(FurnaceState& aFurnace) :
            BaseState(aFurnace, StateId::COMPLETED)
        {
        }

        [[nodiscard]] StateId State() const override { return StateId::COMPLETED; }
    };

    class CancelledState : public BaseState
    {
    public:
        explicit CancelledState(FurnaceState& aFurnace) :
            BaseState(aFurnace, StateId::CANCELLED)
        {
        }

        [[nodiscard]] StateId State() const override { return StateId::CANCELLED; }
    };

    class ErrorState : public BaseState
    {
    public:
        explicit ErrorState(FurnaceState& aFurnace) :
            BaseState(aFurnace, StateId::ERROR)
        {
        }

        [[nodiscard]] StateId State() const override { return StateId::ERROR; }
    };

    class WaitingForTempState : public BaseState
    {
    public:
        explicit WaitingForTempState(FurnaceState& aFurnace) :
            BaseState(aFurnace, StateId::WAITING_FOR_TEMP)
        {
        }

        [[nodiscard]] StateId State() const override { return StateId::WAITING_FOR_TEMP; }
    };
} //namespace furnace

#endif //HEAT_TREAT_FURNACE_STATE_HPP
