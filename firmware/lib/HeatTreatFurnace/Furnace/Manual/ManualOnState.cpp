#include "ManualOnState.hpp"
#include "../StateId.hpp"
#include "Furnace/FurnaceFsm.hpp"

namespace HeatTreatFurnace::Furnace
{
    etl::fsm_state_id_t ManualOnState::on_enter_state()
    {
        auto& fsm = get_fsm_context();
        fsm.SetHeaterOn();
        fsm.SendLog(Log::LogLevel::Info, *this, "Entered MANUAL_TEMP state");
        return No_State_Change;
    }

    void ManualOnState::on_exit_state()
    {
        auto& fsm = get_fsm_context();
        fsm.SetHeaterOff();
        get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Exiting MANUAL_TEMP state");
    }

    etl::fsm_state_id_t ManualOnState::on_event(EvtManualSetTemp const& anEvent)
    {
        auto& fsm = get_fsm_context();
        fsm.SetHeaterTarget(anEvent.targetTemp);
        return No_State_Change;
    }

    etl::fsm_state_id_t ManualOnState::on_event(EvtManualSetOff const& anEvent)
    {
        return STATE_MANUAL_OFF;
    }

    etl::fsm_state_id_t ManualOnState::on_event(EvtModeProfile const& anEvent)
    {
        return STATE_PROFILE;
    }

    etl::fsm_state_id_t ManualOnState::on_event(EvtModeOff const& anEvent)
    {
        auto& fsm = get_fsm_context();
        fsm.SetHeaterOff();
        return STATE_PROFILE;
    }

    etl::fsm_state_id_t ManualOnState::on_event(EvtError const& anEvent)
    {
        auto& fsm = get_fsm_context();
        fsm.SendLog(Log::LogLevel::Error, *this, "Received EvtError: {}", anEvent.msg);

        return STATE_ERROR;
    }

    etl::fsm_state_id_t ManualOnState::on_event_unknown(etl::imessage const& aMsg)
    {
        auto& fsm = get_fsm_context();
        fsm.SendLog(Log::LogLevel::Warn, *this, "Received unknown event with ID: {}", static_cast<int>(aMsg.get_message_id()));
        return No_State_Change;
    }

    StateName ManualOnState::Name() const
    {
        return "ManualOnState";
    }
} // namespace HeatTreatFurnace::FSM

