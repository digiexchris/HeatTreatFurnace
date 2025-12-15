//
// Created by chris on 12/14/25.
//

#include "ProfileFsm.hpp"

namespace HeatTreatFurnace::Profile
{
    ProfileFsm::ProfileFsm(Log::LogService& aLogger) :
        fsm(PROFILE_FSM_ROUTER), Loggable(aLogger), myLogger(aLogger)
    {
    }

    void ProfileFsm::Init()
    {
        set_states(myStatePack);
        start();
    }

    StateId ProfileFsm::GetCurrentState() const noexcept
    {
        etl::fsm_state_id_t stateId = get_state_id();
        return static_cast<StateId>(stateId);
    }
}

}
