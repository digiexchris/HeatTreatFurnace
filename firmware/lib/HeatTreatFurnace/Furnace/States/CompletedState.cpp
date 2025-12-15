#include "CompletedState.hpp"
#include "StateId.hpp"
#include "FSM/FurnaceFsm.hpp"

namespace HeatTreatFurnace::FSM
{
etl::fsm_state_id_t CompletedState::on_enter_state()
{
    get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Entered COMPLETED state");
    return No_State_Change;
}

void CompletedState::on_exit_state()
{
    get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Exiting COMPLETED state");
}

etl::fsm_state_id_t CompletedState::on_event(EvtClearProgram const& anEvent)
{
    etl::fsm_state_id_t result = No_State_Change;

    get_fsm_context().SendLog(Log::LogLevel::Debug, *this, "Received EvtClearProgram");

    // TODO: Clear program and completion status
    // TODO: Perform any cleanup

    get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Program cleared, returning to IDLE");
    result = static_cast<etl::fsm_state_id_t>(StateId::IDLE);

    return result;
}

etl::fsm_state_id_t CompletedState::on_event(EvtSetManualTemp const& anEvent)
{
    etl::fsm_state_id_t result = No_State_Change;

    get_fsm_context().SendLog(Log::LogLevel::Debug, *this, "Received EvtSetManualTemp, target: {} C", anEvent.targetTemp);

    // TODO: Store manual temperature setpoint
    // TODO: Transition to manual temperature control mode

    get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Transitioning to MANUAL_TEMP mode");
    result = static_cast<etl::fsm_state_id_t>(StateId::MANUAL_TEMP);

    return result;
}

etl::fsm_state_id_t CompletedState::on_event(EvtLoadProfile const& anEvent)
{
    etl::fsm_state_id_t result = No_State_Change;

    get_fsm_context().SendLog(Log::LogLevel::Debug, *this, "Received EvtLoadProfile");

    // TODO: Load new profile
    // Profile is owned by FurnaceState, not FSM

    get_fsm_context().SendLog(Log::LogLevel::Info, *this, "Profile loaded, transitioning to LOADED");
    result = static_cast<etl::fsm_state_id_t>(StateId::LOADED);

    return result;
}

etl::fsm_state_id_t CompletedState::on_event(EvtError const& anEvent)
{
    etl::fsm_state_id_t result = No_State_Change;

    get_fsm_context().SendLog(Log::LogLevel::Error, *this, "Received EvtError: {}", anEvent.msg);

    result = static_cast<etl::fsm_state_id_t>(StateId::ERROR);

    return result;
}

etl::fsm_state_id_t CompletedState::on_event_unknown(etl::imessage const& aMsg)
{
    etl::fsm_state_id_t result = No_State_Change;

    get_fsm_context().SendLog(Log::LogLevel::Warn, *this, "Received unknown event with ID: {}", static_cast<int>(aMsg.get_message_id()));
    result = No_State_Change;

    return result;
}

StateName CompletedState::Name() const
{
    return "CompletedState";
}

} // namespace HeatTreatFurnace::FSM

