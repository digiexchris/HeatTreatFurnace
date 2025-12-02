#include <CppUTest/TestHarness.h>
#include "Furnace/StateMachine.hpp"
#include "Furnace/State.hpp"

TEST_GROUP(StateMachine) {};

TEST(StateMachine, InitialState)
{
    StateMachine sm(State::IDLE);
    CHECK_EQUAL(State::IDLE, sm.GetState());
}

TEST(StateMachine, ValidTransition)
{
    StateMachine sm(State::IDLE);
    CHECK(sm.CanTransition(State::LOADED));
    CHECK(sm.TryTransition(State::LOADED));
    CHECK_EQUAL(State::LOADED, sm.GetState());
}

TEST(StateMachine, InvalidTransition)
{
    StateMachine sm(State::IDLE);
    CHECK(!sm.CanTransition(State::RUNNING));
    CHECK(!sm.TryTransition(State::RUNNING));
    CHECK_EQUAL(State::IDLE, sm.GetState());
}

TEST(StateMachine, AdditionalVerification)
{
    class CustomStateMachine : public StateMachine
    {
    public:
        CustomStateMachine(State aInitialState) : StateMachine(aInitialState) {}
    protected:
        bool PrivVerifyTransition(State aFromState, State aToState) const override
        {
            // Disallow transition to ERROR for test
            return aToState != State::ERROR;
        }
    };
    CustomStateMachine sm(State::IDLE);
    CHECK(sm.CanTransition(State::ERROR));
    CHECK(!sm.TryTransition(State::ERROR));
    CHECK_EQUAL(State::IDLE, sm.GetState());
}
