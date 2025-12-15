#include "ManualState.hpp"
#include "../StateId.hpp"
#include "Furnace/FurnaceFsm.hpp"

namespace HeatTreatFurnace::Furnace
{
    etl::fsm_state_id_t ManualState::on_enter_state()
    {
        get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Entered MANUAL_TEMP state");
        return No_State_Change;
    }

    void ManualState::on_exit_state()
    {
        get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Exiting MANUAL_TEMP state");
    }

    //ie resume stopped or loaded fresh profile
    etl::fsm_state_id_t ManualState::on_event(EvtProfileStart const& anEvent)
    {
        etl::fsm_state_id_t result = No_State_Change;

        get_fsm_context().SendLog(Log::LogLevel::Debug, *this, "Received EvtProfileStart");

        return STATE_PROFILE_RUNNING;
    }

    etl::fsm_state_id_t ManualState::on_event(EvtManualSetTemp const& anEvent)
    {
        auto& fsm = get_fsm_context();
        fsm.SendLog(Log::LogLevel::Debug, *this, "Received EvtManualSetTemp, new target: {} C", anEvent.targetTemp);

        fsm.SetHeaterTarget(anEvent.targetTemp);
        return No_State_Change;
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

