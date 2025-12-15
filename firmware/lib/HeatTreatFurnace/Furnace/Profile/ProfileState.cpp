//
// Created by chris on 12/14/25.
//

#include "ProfileState.hpp"
#include "Furnace/StateId.hpp"
#include "Furnace/Events.hpp"
#include "Furnace/FurnaceFsm.hpp"

namespace HeatTreatFurnace::Furnace
{
    etl::fsm_state_id_t ProfileState::on_enter_state()
    {
        auto& fsm = get_fsm_context();
        if (fsm.IsHeaterOn())
        {
            fsm.SetHeaterOff();
            EvtError evt(Error::SafetyInterlock, Domain::Furnace, "Entering ProfileState with the heater already on, this should never happen.");
            fsm.Post(evt, EventPriority::Critical);
        }
        if (fsm.GetCurrentProfile() != nullptr)
        {
            EvtProfileAlreadyLoaded evt;
            fsm.Post(evt, EventPriority::Furnace);
        }
        return No_State_Change;
    }

    void ProfileState::on_exit_state()
    {
    }

    etl::fsm_state_id_t ProfileState::on_event(EvtProfileLoad const& anEvent)
    {
        //TODO call the handler to load the new program
        return STATE_PROFILE_LOADED;
    }

    etl::fsm_state_id_t ProfileState::on_event(EvtManualSetTemp const& anEvent)
    {
        auto& fsm = get_fsm_context();
        fsm.SetHeaterTarget(anEvent.targetTemp);
        fsm.SendLog(Log::LogLevel::Debug, *this, "Manual temperature set event received, transitioning to manual mode");
        return STATE_MANUAL;
    }

    //transitions to MANUAL, with the current setpoint set.
    //TODO the current set point will need to be shown in the UI.
    etl::fsm_state_id_t ProfileState::on_event(EvtModeManual const& anEvent)
    {
        return STATE_MANUAL;
    }

    etl::fsm_state_id_t ProfileState::on_event(EvtModeOff const& anEvent)
    {
        return STATE_OFF;
    }

    etl::fsm_state_id_t ProfileState::on_event(EvtTick const& anEvent)
    {
        //no op, just making it valid for this event to be sent to this FSM always.
        return No_State_Change;
    }

    //unknown events hitting this FSM is always ignored.
    etl::fsm_state_id_t ProfileState::on_event_unknown(etl::imessage const& aMsg)
    {
        auto& fsm = get_fsm_context();
        fsm.SendLog(Log::LogLevel::Debug, *this, "Unknown event received, ignoring");
        return No_State_Change;
    }

    etl::fsm_state_id_t ProfileState::onEvent(EvtError const& anEvent)
    {
        return STATE_ERROR;
    }

    StateName ProfileState::Name() const
    {
        return "Profile::ProfileState";
    }
}
