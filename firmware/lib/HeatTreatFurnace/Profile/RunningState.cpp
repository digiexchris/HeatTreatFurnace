//
// Created by chris on 12/14/25.
//

#include "ProfileFsm.hpp"
#include "States.hpp"
#include "Log/LogLevel.hpp"

namespace HeatTreatFurnace::Profile
{
    etl::fsm_state_id_t RunningState::on_enter_state()
    {
        //todo check that the program is valid
        //todo check that the current position is valid or not at the end
        //todo set the target to the current profile position target via the main FSM
        //todo turn the heater on via the main FSM
        //if any of those fail, return STATE_IDLE and log an error and post an Error event to the main FSM.
        return STATE_RUNNING;
    }

    void RunningState::on_exit_state()
    {
        //TODO always set the heater to off via the main FSM.
    }

    etl::fsm_state_id_t RunningState::on_event(EvtStop const& anEvent)
    {
        //emit heater off event to main FSM
        //emit program stopped event to main FSM?? maybe not, it controls this so it would know... it can check if it was successfully changed.
        return STATE_IDLE;
    }

    etl::fsm_state_id_t RunningState::on_event(EvtSetNextSegment const& anEvent)
    {
        //validate the segment
        //update the myCurrentProgram to the expected segment and time
        return No_State_Change;
    }

    etl::fsm_state_id_t RunningState::on_event(EvtTick const& anEvent)
    {
        ProfileFsm& fsm = get_fsm_context();
        ProfileFsm::ProfileUpdateResult result = fsm.UpdateNextProfileTemp();
        etl::fsm_state_id_t nextState = No_State_Change;
        if (result == ProfileFsm::ProfileUpdateResult::END)
        {
            //TODO emit completed event to main FSM
            //turn off heater via main fsm
            fsm.SendLog(Log::LogLevel::Debug, *this, "Program completed");
            nextState = STATE_IDLE;
        }

        return nextState;
    }

    etl::fsm_state_id_t RunningState::on_event_unknown(etl::imessage const& aMsg)
    {
        ProfileFsm& fsm = get_fsm_context();
        fsm.SendLog(Log::LogLevel::Debug, *this, "Unknown event received, stopping profile");
        return STATE_IDLE;
    }

    StateName RunningState::Name() const
    {
        return "Profile::RunningState";
    }
}
