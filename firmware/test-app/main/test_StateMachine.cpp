#include <catch2/catch_test_macros.hpp>
#include <catch2/trompeloeil.hpp>

#include "Furnace/StateMachine.hpp"
#include "Furnace/State.hpp"
#include "Furnace/Result.hpp"
#include "Log/Log.hpp"
#include <memory>
#include <map>

class MockBaseState : public BaseState
{
public:
    MAKE_MOCK0(State, StateId(), const override);
    MAKE_MOCK0(OnEnter, Result(), override);
    MAKE_MOCK0(OnExit, Result(), override);

    explicit MockBaseState(Furnace* aFurnace) :
        BaseState(aFurnace)
    {
    }
};

class MockLogCallback
{
public:
    std::string logMsg;

    static void LogCallback(const spdlog::details::log_msg& msg)
    {
        logMsg = msg.payload.data();

    }
};

class StateMachineFixture
{
public:
    Log* myLog;
    Log::Config myLogConfig;
    MockLogCallback* mockLogCallback;

    StateMachineFixture()
    {
        mockLogCallback = new MockLogCallback();
        myLogConfig.callback.enable = true;
        myLogConfig.callback.callback = mockLogCallback->LogCallback;
        myLog = new Log(myLogConfig);
    }

    ~StateMachineFixture()
    {
        delete myLog;
    }
};

TEST_CASE_METHOD(StateMachineFixture, "StateMachine: Constructor - initializes to IDLE state")
{
    StateMachine stateMachine(nullptr, myLog);
    REQUIRE(stateMachine.GetState() == StateId::IDLE);
}

TEST_CASE_METHOD(StateMachineFixture, "StateMachine: GetState - returns current state")
{
    StateMachine stateMachine(nullptr, myLog);

    SECTION("Returns IDLE after construction")
    {
        REQUIRE(stateMachine.GetState() == StateId::IDLE);
    }

    SECTION("Updates after successful transition")
    {
        REQUIRE(stateMachine.TransitionTo(StateId::LOADED));
        REQUIRE(stateMachine.GetState() == StateId::LOADED);
    }
}

TEST_CASE_METHOD(StateMachineFixture, "StateMachine: CanTransition - valid transitions are allowed")
{
    StateMachine stateMachine(nullptr, myLog);

    SECTION("From IDLE state")
    {
        REQUIRE(stateMachine.GetState() == StateId::IDLE);
        REQUIRE(stateMachine.CanTransition(StateId::LOADED));
        REQUIRE(stateMachine.CanTransition(StateId::ERROR));
    }

    SECTION("From LOADED state")
    {
        stateMachine.TransitionTo(StateId::LOADED);
        REQUIRE(stateMachine.GetState() == StateId::LOADED);
        REQUIRE(stateMachine.CanTransition(StateId::IDLE));
        REQUIRE(stateMachine.CanTransition(StateId::RUNNING));
        REQUIRE(stateMachine.CanTransition(StateId::ERROR));
    }

    SECTION("From RUNNING state")
    {
        stateMachine.TransitionTo(StateId::LOADED);
        stateMachine.TransitionTo(StateId::RUNNING);
        REQUIRE(stateMachine.GetState() == StateId::RUNNING);
        REQUIRE(stateMachine.CanTransition(StateId::PAUSED));
        REQUIRE(stateMachine.CanTransition(StateId::COMPLETED));
        REQUIRE(stateMachine.CanTransition(StateId::CANCELLED));
        REQUIRE(stateMachine.CanTransition(StateId::ERROR));
    }

    SECTION("From PAUSED state")
    {
        stateMachine.TransitionTo(StateId::LOADED);
        stateMachine.TransitionTo(StateId::RUNNING);
        stateMachine.TransitionTo(StateId::PAUSED);
        REQUIRE(stateMachine.GetState() == StateId::PAUSED);
        REQUIRE(stateMachine.CanTransition(StateId::RUNNING));
        REQUIRE(stateMachine.CanTransition(StateId::CANCELLED));
        REQUIRE(stateMachine.CanTransition(StateId::ERROR));
    }

    SECTION("From COMPLETED state")
    {
        stateMachine.TransitionTo(StateId::LOADED);
        stateMachine.TransitionTo(StateId::RUNNING);
        stateMachine.TransitionTo(StateId::COMPLETED);
        REQUIRE(stateMachine.GetState() == StateId::COMPLETED);
        REQUIRE(stateMachine.CanTransition(StateId::IDLE));
        REQUIRE(stateMachine.CanTransition(StateId::LOADED));
        REQUIRE(stateMachine.CanTransition(StateId::ERROR));
    }

    SECTION("From CANCELLED state")
    {
        stateMachine.TransitionTo(StateId::LOADED);
        stateMachine.TransitionTo(StateId::RUNNING);
        stateMachine.TransitionTo(StateId::CANCELLED);
        REQUIRE(stateMachine.GetState() == StateId::CANCELLED);
        REQUIRE(stateMachine.CanTransition(StateId::IDLE));
        REQUIRE(stateMachine.CanTransition(StateId::LOADED));
        REQUIRE(stateMachine.CanTransition(StateId::ERROR));
    }

    SECTION("From ERROR state")
    {
        stateMachine.TransitionTo(StateId::ERROR);
        REQUIRE(stateMachine.GetState() == StateId::ERROR);
        REQUIRE(stateMachine.CanTransition(StateId::IDLE));
        REQUIRE(stateMachine.CanTransition(StateId::LOADED));
    }

    SECTION("From WAITING_FOR_TEMP state")
    {
        stateMachine.TransitionTo(StateId::LOADED);
        stateMachine.TransitionTo(StateId::RUNNING);
        stateMachine.TransitionTo(StateId::WAITING_FOR_TEMP);
        REQUIRE(stateMachine.GetState() == StateId::WAITING_FOR_TEMP);
        REQUIRE(stateMachine.CanTransition(StateId::RUNNING));
        REQUIRE(stateMachine.CanTransition(StateId::PAUSED));
        REQUIRE(stateMachine.CanTransition(StateId::ERROR));
    }
}

TEST_CASE_METHOD(StateMachineFixture, "StateMachine: CanTransition - invalid transitions are rejected")
{
    StateMachine stateMachine(nullptr, myLog);

    SECTION("IDLE cannot transition to RUNNING")
    {
        REQUIRE_FALSE(stateMachine.CanTransition(StateId::RUNNING));
    }

    SECTION("IDLE cannot transition to PAUSED")
    {
        REQUIRE_FALSE(stateMachine.CanTransition(StateId::PAUSED));
    }

    SECTION("RUNNING cannot transition to IDLE")
    {
        stateMachine.TransitionTo(StateId::LOADED);
        stateMachine.TransitionTo(StateId::RUNNING);
        REQUIRE_FALSE(stateMachine.CanTransition(StateId::IDLE));
    }

    SECTION("RUNNING cannot transition to LOADED")
    {
        stateMachine.TransitionTo(StateId::LOADED);
        stateMachine.TransitionTo(StateId::RUNNING);
        REQUIRE_FALSE(stateMachine.CanTransition(StateId::LOADED));
    }

    SECTION("COMPLETED cannot transition to RUNNING")
    {
        stateMachine.TransitionTo(StateId::LOADED);
        stateMachine.TransitionTo(StateId::RUNNING);
        stateMachine.TransitionTo(StateId::COMPLETED);
        REQUIRE_FALSE(stateMachine.CanTransition(StateId::RUNNING));
    }

    SECTION("CANCELLED cannot transition to RUNNING")
    {
        stateMachine.TransitionTo(StateId::LOADED);
        stateMachine.TransitionTo(StateId::RUNNING);
        stateMachine.TransitionTo(StateId::CANCELLED);
        REQUIRE_FALSE(stateMachine.CanTransition(StateId::RUNNING));
    }

    SECTION("ERROR cannot transition to RUNNING")
    {
        stateMachine.TransitionTo(StateId::ERROR);
        REQUIRE_FALSE(stateMachine.CanTransition(StateId::RUNNING));
    }

    SECTION("WAITING_FOR_TEMP cannot transition to IDLE")
    {
        stateMachine.TransitionTo(StateId::LOADED);
        stateMachine.TransitionTo(StateId::RUNNING);
        stateMachine.TransitionTo(StateId::WAITING_FOR_TEMP);
        REQUIRE_FALSE(stateMachine.CanTransition(StateId::IDLE));
    }
}

TEST_CASE_METHOD(StateMachineFixture, "StateMachine: TransitionTo - successful transition calls OnExit then OnEnter")
{
    using trompeloeil::_;

    Furnace* mockFurnace = nullptr;
    auto mockIdleState = std::make_unique<MockBaseState>(mockFurnace);
    auto mockLoadedState = std::make_unique<MockBaseState>(mockFurnace);

    REQUIRE_CALL(*mockIdleState, State())
        .RETURN(StateId::IDLE);
    REQUIRE_CALL(*mockIdleState, OnExit())
        .RETURN(Result{true, ""});

    REQUIRE_CALL(*mockLoadedState, State())
        .RETURN(StateId::LOADED);
    REQUIRE_CALL(*mockLoadedState, OnEnter())
        .RETURN(Result{true, ""});

    StateMachine::StateMap mockStates;
    mockStates.insert({StateId::IDLE, std::move(mockIdleState)});
    mockStates.insert({StateId::LOADED, std::move(mockLoadedState)});

    StateMachine stateMachine(mockFurnace, myLog, std::move(mockStates));

    REQUIRE(stateMachine.GetState() == StateId::IDLE);
    REQUIRE(stateMachine.TransitionTo(StateId::LOADED));
    REQUIRE(stateMachine.GetState() == StateId::LOADED);
}

TEST_CASE_METHOD(StateMachineFixture, "StateMachine: TransitionTo - multiple sequential transitions")
{
    StateMachine stateMachine(nullptr, myLog);

    REQUIRE(stateMachine.GetState() == StateId::IDLE);
    REQUIRE(stateMachine.TransitionTo(StateId::LOADED));
    REQUIRE(stateMachine.GetState() == StateId::LOADED);
    REQUIRE(stateMachine.TransitionTo(StateId::RUNNING));
    REQUIRE(stateMachine.GetState() == StateId::RUNNING);
    REQUIRE(stateMachine.TransitionTo(StateId::PAUSED));
    REQUIRE(stateMachine.GetState() == StateId::PAUSED);
    REQUIRE(stateMachine.TransitionTo(StateId::RUNNING));
    REQUIRE(stateMachine.GetState() == StateId::RUNNING);
}

TEST_CASE_METHOD(StateMachineFixture, "StateMachine: TransitionTo - invalid transition returns false")
{
    StateMachine stateMachine(nullptr, myLog);

    REQUIRE(stateMachine.GetState() == StateId::IDLE);
    REQUIRE_FALSE(stateMachine.TransitionTo(StateId::RUNNING));
    REQUIRE(stateMachine.GetState() == StateId::IDLE);
}

TEST_CASE_METHOD(StateMachineFixture, "StateMachine: TransitionTo - OnExit failure transitions to ERROR")
{
    using trompeloeil::_;

    Furnace* mockFurnace = nullptr;
    auto mockIdleState = std::make_unique<MockBaseState>(mockFurnace);
    auto mockErrorState = std::make_unique<MockBaseState>(mockFurnace);

    REQUIRE_CALL(*mockIdleState, State())
        .RETURN(StateId::IDLE);
    REQUIRE_CALL(*mockIdleState, OnExit())
        .RETURN(Result{false, "OnExit failed"});

    REQUIRE_CALL(*mockErrorState, State())
        .RETURN(StateId::ERROR);
    REQUIRE_CALL(*mockErrorState, OnEnter())
        .RETURN(Result{true, ""});

    StateMachine::StateMap mockStates;
    mockStates.insert({StateId::IDLE, std::move(mockIdleState)});
    mockStates.insert({StateId::ERROR, std::move(mockErrorState)});

    StateMachine stateMachine(mockFurnace, myLog, std::move(mockStates));

    REQUIRE(stateMachine.GetState() == StateId::IDLE);
    REQUIRE_FALSE(stateMachine.TransitionTo(StateId::LOADED));
    REQUIRE(stateMachine.GetState() == StateId::ERROR);
}

TEST_CASE_METHOD(StateMachineFixture, "StateMachine: TransitionTo - OnEnter failure transitions to ERROR")
{
    using trompeloeil::_;

    Furnace* mockFurnace = nullptr;
    auto mockIdleState = std::make_unique<MockBaseState>(mockFurnace);
    auto mockLoadedState = std::make_unique<MockBaseState>(mockFurnace);
    auto mockErrorState = std::make_unique<MockBaseState>(mockFurnace);

    REQUIRE_CALL(*mockIdleState, State())
        .RETURN(StateId::IDLE);
    REQUIRE_CALL(*mockIdleState, OnExit())
        .RETURN(Result{true, ""});

    REQUIRE_CALL(*mockLoadedState, State())
        .RETURN(StateId::LOADED);
    REQUIRE_CALL(*mockLoadedState, OnEnter())
        .RETURN(Result{false, "OnEnter failed"});

    REQUIRE_CALL(*mockErrorState, State())
        .RETURN(StateId::ERROR);
    REQUIRE_CALL(*mockErrorState, OnEnter())
        .RETURN(Result{true, ""});

    StateMachine::StateMap mockStates;
    mockStates.insert({StateId::IDLE, std::move(mockIdleState)});
    mockStates.insert({StateId::LOADED, std::move(mockLoadedState)});
    mockStates.insert({StateId::ERROR, std::move(mockErrorState)});

    StateMachine stateMachine(mockFurnace, myLog, std::move(mockStates));

    REQUIRE(stateMachine.GetState() == StateId::IDLE);
    REQUIRE_FALSE(stateMachine.TransitionTo(StateId::LOADED));
    REQUIRE(stateMachine.GetState() == StateId::ERROR);
}
