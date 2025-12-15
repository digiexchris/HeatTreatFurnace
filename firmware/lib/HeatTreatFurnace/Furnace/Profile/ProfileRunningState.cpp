//
// Created by chris on 12/14/25.
//

#include "ProfileRunningState.hpp"

#include "Furnace/StateId.hpp"
#include "Furnace/Events.hpp"
#include "Furnace/FurnaceFsm.hpp"
#include "Log/LogLevel.hpp"

namespace HeatTreatFurnace::Furnace
{
    etl::fsm_state_id_t ProfileRunningState::on_enter_state()
    {
        FurnaceFsm& fsm = get_fsm_context();

        if (fsm.GetCurrentProfile() == nullptr)
        {
            //Transition immediately to Profile with no loaded profile.
            EvtModeProfile evt;
            fsm.Post(evt, EventPriority::Furnace);
        }
        else
        {
            auto res = fsm.UpdateNextProfileTempTarget();
            if (res == FurnaceFsm::ProfileUpdateResult::END)
            {
                fsm.HandleProfileCompleted(res);
            }
            else
            {
                fsm.SetHeaterOn();
            }
        }

        return No_State_Change;
    }

    void ProfileRunningState::on_exit_state()
    {
        auto& fsm = get_fsm_context();
        fsm.SetHeaterOff();
    }

    etl::fsm_state_id_t ProfileRunningState::on_event(EvtModeOff const& anEvent)
    {
        return STATE_OFF;
    }

    etl::fsm_state_id_t ProfileRunningState::on_event(EvtManualSetTemp const& anEvent)
    {
        auto& fsm = get_fsm_context();
        return fsm.HandleEvent(anEvent);
    }

    etl::fsm_state_id_t ProfileRunningState::on_event(EvtProfileStop const& anEvent)
    {
        //The exit state takes care of the heater, so we just need to transition.
        return STATE_PROFILE_STOPPED;
    }

    etl::fsm_state_id_t ProfileRunningState::on_event(EvtProfileSetNextSegment const& anEvent)
    {
        //TODO validate the segment
        //TODO update the myCurrentProgram to the expected segment and time
        //TODO auto res = fsm.UpdateNextProfileTempTarget();
        return No_State_Change;
    }

    etl::fsm_state_id_t ProfileRunningState::on_event(EvtTick const& anEvent)
    {
        auto& fsm = get_fsm_context();
        auto result = fsm.UpdateNextProfileTempTarget();
        fsm.HandleProfileCompleted(result);
        return No_State_Change;
    }

    etl::fsm_state_id_t ProfileRunningState::on_event_unknown(etl::imessage const& aMsg)
    {
        auto& fsm = get_fsm_context();
        fsm.SendLog(Log::LogLevel::Debug, *this, "Unknown event received, stopping profile");
        return No_State_Change;
    }

    StateName ProfileRunningState::Name() const
    {
        return "Profile::ProfileRunningState";
    }
}
