#pragma once

#include "Profile.hpp"
#include "State.hpp"

class StateMachine
{
public:
    StateMachine();
    StateId GetState() const;
    bool CanTransition(StateId aToState) const;
    bool TransitionTo(StateId aToState);

    //Actions

protected:
    std::map<StateId, std::shared_ptr<BaseState>> myStates;
    std::shared_ptr<StateMachine> myInstance;

private:
    StateId myCurrentState;
    std::shared_ptr<Profile> myLoadedProfile;

    //The Action would have asked that the loaded profile be replaced with this, which will happen when the Load() transition happens.
    std::shared_ptr<Profile> myProfileToLoad;

};

