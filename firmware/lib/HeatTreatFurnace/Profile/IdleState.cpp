//
// Created by chris on 12/14/25.
//

#include "ProfileFsm.hpp"
#include "States.hpp"
#include "Events.hpp"

namespace HeatTreatFurnace::Profile
{
    etl::fsm_state_id_t IdleState::on_enter_state()
    {
        return STATE_IDLE;
    }

    void IdleState::on_exit_state()
    {
    }

    etl::fsm_state_id_t IdleState::on_event(EvtLoadProfile const& anEvent)
    {
        return STATE_IDLE;
    }

    etl::fsm_state_id_t IdleState::on_event(EvtStart const& anEvent)
    {
        ProfileFsm& fsm = get_fsm_context();
        etl::fsm_state_id_t nextState = STATE_RUNNING;
        auto currentProgram = fsm.myCurrentProfile;
        if (currentProgram != nullptr)
        {
            fsm.SendLog(Log::LogLevel::Debug, *this, "Starting program: {}", currentProgram->name);
        }
        else
        {
            fsm.SendLog(Log::LogLevel::Debug, *this, "No program loaded, cannot start");
            nextState = No_State_Change;
        }

        return nextState;
    }

    etl::fsm_state_id_t IdleState::on_event(EvtClearProgram const& anEvent)
    {
        ProfileFsm& fsm = get_fsm_context();
        auto result = fsm.ClearProgram();
        if (result)
        {
            fsm.SendLog(Log::LogLevel::Debug, *this, "Program cleared");
        }
        else
        {
            fsm.SendLog(Log::LogLevel::Debug, *this, "Failed to clear program, one probably wasn't loaded. Ignoring");
        }
        return No_State_Change;
    }

    etl::fsm_state_id_t IdleState::on_event(EvtSetNextSegment const& anEvent)
    {
        ProfileFsm& fsm = get_fsm_context();
        auto result = fsm.SetProgramPosition(anEvent.segmentIndex, anEvent.segmentTimePosition);
        if (result)
        {
            fsm.SendLog(Log::LogLevel::Debug, *this, "Program position changed to Segment {}, {}s", anEvent.segmentIndex,
                        anEvent.segmentTimePosition.count());
        }
        return No_State_Change;
    }

    etl::fsm_state_id_t IdleState::on_event(EvtTick const& anEvent)
    {
        //no op, just making it valid for this event to be sent to this FSM always.
        return No_State_Change;
    }

    //unknown events hitting this FSM is always an error.
    etl::fsm_state_id_t IdleState::on_event_unknown(etl::imessage const& aMsg)
    {
        ProfileFsm& fsm = get_fsm_context();
        fsm.SendLog(Log::LogLevel::Debug, *this, "Unknown event received, ignoring");
        return STATE_IDLE;
    }

    StateName IdleState::Name() const
    {
        return "Profile::IdleState";
    }
}
