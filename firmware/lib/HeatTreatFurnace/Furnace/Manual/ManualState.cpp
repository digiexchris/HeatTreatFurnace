#include "ManualState.hpp"
#include "../StateId.hpp"
#include "Furnace/FurnaceFsm.hpp"

namespace HeatTreatFurnace::Furnace
{
    etl::fsm_state_id_t ManualState::on_enter_state()
    {
        auto& fsm = get_fsm_context();
        fsm.SendLog(Log::LogLevel::Info, *this, "Entered MANUAL_TEMP state");
        if (fsm.IsHeaterOn())
        {
            fsm.SetHeaterOff();
            EvtError evt(Error::SafetyInterlock, Domain::Furnace, "Entering ManualState with the heater already on, this should never happen.");
            fsm.Post(evt);
        }
        fsm.Post(EvtManualSetOff());
        return No_State_Change;
    }

    void ManualState::on_exit_state()
    {
        auto& fsm = get_fsm_context();
        if (fsm.IsHeaterOn())
        {
            fsm.SetHeaterOff();
            EvtError evt(Error::SafetyInterlock, Domain::Furnace, "Exiting ManualState with the heater already on, this should never happen.");
            fsm.Post(evt);
        }
        get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Exiting MANUAL_TEMP state");
    }

    etl::fsm_state_id_t ManualState::on_event(EvtManualSetOff const& anEvent)
    {
        return STATE_MANUAL_OFF;
    }

    etl::fsm_state_id_t ManualState::on_event(EvtModeProfile const& anEvent)
    {
        return STATE_PROFILE;
    }

    etl::fsm_state_id_t ManualState::on_event(EvtModeOff const& anEvent)
    {
        auto& fsm = get_fsm_context();
        fsm.SetHeaterOff();
        return STATE_OFF;
    }

    etl::fsm_state_id_t ManualState::on_event(EvtError const& anEvent)
    {
        auto& fsm = get_fsm_context();
        fsm.SendLog(Log::LogLevel::Error, *this, "Received EvtError: {}", anEvent.msg);

        return STATE_ERROR;
    }

    etl::fsm_state_id_t ManualState::on_event_unknown(etl::imessage const& aMsg)
    {
        auto& fsm = get_fsm_context();
        fsm.SendLog(Log::LogLevel::Warn, *this, "Received unknown event with ID: {}", static_cast<int>(aMsg.get_message_id()));
        return No_State_Change;
    }

    StateName ManualState::Name() const
    {
        return "ManualState";
    }
} // namespace HeatTreatFurnace::FSM

