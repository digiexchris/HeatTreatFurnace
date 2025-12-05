#pragma once

#include "Profile.hpp"
#include "State.hpp"

class Furnace;
class LogService;

class StateMachine
{
public:
    using StateMap = std::map<StateId, std::unique_ptr<BaseState>>;

    /** @brief State Machine dependencies:
     * StateMap will be moved to myState
     */
    explicit StateMachine(Furnace* aFurnace, LogService* aLog, StateMap aStateMap = StateMap());
    ~StateMachine() = default;
    [[nodiscard]] StateId GetState() const;
    [[nodiscard]] bool CanTransition(const StateId& aToState);
    bool TransitionTo(StateId aToState);

    //Actions

protected:
    std::map<StateId, std::unique_ptr<BaseState>> myStates;

    static StateMap CreateDefaultStates(Furnace* furnace);

private:
    StateId myCurrentState;
    std::unique_ptr<Profile> myLoadedProfile;

    //The Action would have asked that the loaded profile be replaced with this, which will happen when the Load() transition happens.
    std::unique_ptr<Profile> myProfileToLoad;
    LogService* myLog;
};

